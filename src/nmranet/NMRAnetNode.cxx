/** \copyright
 * Copyright (c) 2013, Stuart W Baker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \file NMRAnetNode.cxx
 * This file defines NMRAnet nodes.
 *
 * @author Stuart W. Baker
 * @date 29 September 2013
 */

#include "nmranet/NMRAnetNode.hxx"
#include "utils/BufferQueue.hxx"

namespace NMRAnet
{

OSMutex Node::mutex;
RBTree <NodeID, Node*> Node::idTree;

/** Obtain a @ref Node instance from its Node ID.
 * @param node_id Node ID to lookup
 * @return @ref Node instance, else NULL if not found
 */
Node *Node::find(NodeID node_id)
{
    mutex.lock();
    RBTree <NodeID, Node*>::Node *node = idTree.find(node_id);
    mutex.unlock();
    if (node)
    {
        return node->value;
    }
    return NULL;
}

/** Move node into the initialized state.
 */
void Node::initialized()
{
    mutex.lock();
    if (state != INITIALIZED)
    {
        state = INITIALIZED;
        verify_id_number();
        /* identify all of the events this node produces and consumes */
        //nmranet_identify_consumers(node, 0, EVENT_ALL_MASK);
        //nmranet_identify_producers(node, 0, EVENT_ALL_MASK);
    }
    mutex.unlock();
}

/** Send a verify node id number message.
 */
void Node::verify_id_number()
{
    Buffer *buffer = buffer_alloc(6);
    uint8_t *data = (uint8_t*)buffer->start();
    
    data[0] = (nodeID >> 40) & 0xff;
    data[1] = (nodeID >> 32) & 0xff;
    data[2] = (nodeID >> 24) & 0xff;
    data[3] = (nodeID >> 16) & 0xff;
    data[4] = (nodeID >>  8) & 0xff;
    data[5] = (nodeID >>  0) & 0xff;
    
    buffer->advance(6);
    
    write(If::MTI_VERIFIED_NODE_ID_NUMBER, {0, 0}, buffer);
}

/** Send an ident info reply message.
 * @param dst destination Node ID to respond to
 */
void Node::ident_info_reply(NodeHandle dst)
{
    size_t  size = 8;
    Buffer *buffer;
    char   *pos;

    /* macro for condensing the size calculation code */
    #define ADD_STRING_SIZE(_str, _max)          \
    {                                            \
        if ((_str))                              \
        {                                        \
            size_t len = strlen((_str));         \
            size += len > (_max) ? (_max) : len; \
        }                                        \
    }

    /* macro for condensing the string insertion  code */
    #define INSERT_STRING(_str, _max)              \
    {                                              \
        if ((_str))                                \
        {                                          \
            size_t len = strlen((_str));           \
            len = len > (_max) ? (_max) : len;     \
            memcpy(pos, (_str), len);              \
            pos[len] = '\0';                       \
            pos = (char*)buffer->advance(len + 1); \
        }                                          \
        else                                       \
        {                                          \
            pos[0] = '\0';                         \
            pos = (char*)buffer->advance(1);       \
        }                                          \
    }
    
    ADD_STRING_SIZE(manufacturer, 40);
    ADD_STRING_SIZE(model, 40);
    ADD_STRING_SIZE(hardware_rev, 20);
    ADD_STRING_SIZE(software_rev, 20);
    ADD_STRING_SIZE(userName, 62);
    ADD_STRING_SIZE(userDescription, 63);

    buffer = buffer_alloc(size);
    pos = (char*)buffer->start();
    pos[0] = SIMPLE_NODE_IDENT_VERSION_A;
    pos = (char*)buffer->advance(1);
    
    INSERT_STRING(manufacturer, 40);
    INSERT_STRING(model, 40);
    INSERT_STRING(hardware_rev, 20);
    INSERT_STRING(software_rev, 20);

    pos[0] = SIMPLE_NODE_IDENT_VERSION_B;
    pos = (char*)buffer->advance(1);

    INSERT_STRING(userName, 62);
    INSERT_STRING(userDescription, 63);
    
    write(If::MTI_IDENT_INFO_REPLY, dst, buffer);
}

/** Write a message from a node.  We should already have a mutex lock at this
 * at this point.
 * @param mti Message Type Indicator
 * @param dst destination node ID, 0 if unavailable
 * @param data NMRAnet packet data
 * @return 0 upon success
 */
int Node::write(If::MTI mti, NodeHandle dst, Buffer *data)
{
    /* It is important to note that we unlock the mutex before sending
     * data to an interface.  This is required for local nodes such that
     * we don't have mutex recursion.  This is required for remote nodes so
     * that if the interface write would block, other nodes may continue to
     * send messages using the interface.  NMRAnet does not guarantee message
     * sequencing.
     */

    if (dst.id == 0 && dst.alias == 0)
    {
        /* broacast message */
        mutex.unlock();
        data->reference();
        nmranetIf->rx_data(mti, {nodeID, 0}, 0, data);
        nmranetIf->if_write(mti, nodeID, dst, data);
    }
    else
    {
        RBTree <NodeID, Node*>::Node *node = idTree.find(dst.id);;

        mutex.unlock();
        if (node)
        {
            /* loop-back */
            nmranetIf->rx_data(mti, {nodeID, 0}, dst.id, data);
        }
        else
        {
            nmranetIf->if_write(mti, nodeID, dst, data);
        }
    }
    mutex.lock();

    return 0;
}

};
