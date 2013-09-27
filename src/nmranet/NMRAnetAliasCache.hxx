/** \copyright
 * Copyright (c) 2013, Stuart W Baker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are  permitted provided that the following conditions are met:
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
 * \file NMRAnetAliasCache.hxx
 * This file provides an Alias Caching mechanism.
 *
 * @author Stuart W. Baker
 * @date 21 September 2013
 */

#ifndef _NMRAnetAliasCache_hxx_
#define _NMRAnetAliasCache_hxx_

#include <map>

#include "os/OS.hxx"
#include "nmranet/NMRAnetIf.hxx"
#include "utils/macros.h"
#include "utils/RBTree.hxx"

namespace NMRAnet
{

struct nodekey_compare {
  bool operator() (const NodeAlias& a, const NodeAlias& b) const {
     return a < b;
  } 
};


/** Cache of alias to node id mappings.  The cache is limited to a fixed number
 * of entries at construction.  All the memory for the cache will be allocated
 * at construction time, limited by the maximum number of entries.  Note, there
 * is no mutual exclusion locking mechanism built into this class.  Mutual
 * exclusion must be handled by the user as needed.
 *
 * @todo the class uses RBTree, consider a version that is a linear search for
 * a small number of entries.
 */
class AliasCache
{
public:
    /** Constructor.
     * @param seed starting seed for generation of aliases
     * @param entries maximum number of entries in this cache
     */
    AliasCache(NodeID seed, size_t entries)
        : freeList(NULL),
          metadata(new Metadata[entries]),
          aliasTree(),
          idTree(),
          timeTree(),
          aliasAllocator(),
          //aliasNode(new RBTree <NodeAlias, Metadata*>::Node[entries]),
          //idNode(new RBTree <NodeID, Metadata*>::Node[entries]),
          //timeNode(new RBTree <long long, Metadata*>::Node[entries]),
          //aliasMap(),
          //idMap(),
          //timeMap(),
          seed(seed)
    {
        for (size_t i = 0; i < entries; ++i)
        {
            metadata[i].next = freeList;
            freeList = metadata;
            aliasNode[i].value = metadata;
            idNode[i].value = metadata;
            timeNode[i].value = metadata;
        }

        aliasAllocator.set_entries(entries);
        
        //Allocator <std::pair<NodeAlias, Metadata*>> aAllocator;
        
        aliasMap = new std::map <NodeAlias, Metadata*, nodekey_compare, Allocator <std::pair<NodeAlias, Metadata*>>>(aliasAllocator);
    }

    /** Add an alias to an alias cache.
     * @param id 48-bit NMRAnet Node ID to associate alias with
     * @param alias 12-bit alias associated with Node ID
     */
    void add(NodeID id, NodeAlias alias);
    
    /** Remove an alias from an alias cache.
     * @param alias 12-bit alias associated with Node ID
     */
    void remove(NodeAlias alias);

    /** Lookup a node's alias based on its Node ID.
     * @param id Node ID to look for
     * @return alias that matches the Node ID, else 0 if not found
     */
    NodeAlias lookup(NodeID id);

    /** Lookup a node's ID based on its alias.
     * @param alias alias to look for
     * @return Node ID that matches the alias, else 0 if not found
     */
    NodeID lookup(NodeAlias alias);

    /** Call the given callback function once for each alias tracked.
     * @param callback method to call
     * @param context context pointer to pass to callback
     */
    void for_each(void (*callback)(void*, NodeID, NodeID), void *context);

    /** Generate a 12-bit pseudo-random alias for a givin alias cache.
     * @return pseudo-random 12-bit alias, an alias of zero is invalid
     */
    NodeAlias generate();

private:
    template <typename T> class Allocator
    {
    public:
        union FreeList
        {
            T data;
            FreeList *next;
        };

        FreeList *freeList;
        
        bool init;
        
        size_t entries;
        
        void set_entries(size_t value)
        {
            entries = value;
        }
        
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        
        template <typename U> struct rebind
        {
            typedef Allocator<U> other;
        };

        explicit Allocator()
            : init(false),
              entries(0)
        {
        }
        
        explicit Allocator(Allocator const&)
            : init(false)
        {
        }
        
        ~Allocator()
        {
        }
        
        template <typename U> explicit Allocator(Allocator<U> const&)
        {
        }

        //    address

        T *address(T &r)
        {
            return &r;
        }

        const T *address(const T &r)
        {
            return &r;
        }

        //    memory allocation

        T *allocate(size_t cnt, const void* = 0)
        {
            if (init == false)
            {
                HASSERT(entries != 0);
                init = true;
                FreeList *newList = (FreeList*)malloc(sizeof(FreeList) * max_size());
                newList->next = NULL;
                for (size_t i = 0; i < max_size(); ++i)
                {
                    freeList->next = &newList[i];
                    freeList = &newList[i];
                }
            }
            HASSERT(freeList != NULL);
            HASSERT(cnt == 1);
            
            T *newT = &(freeList->data);
            freeList = freeList->next;
            return newT;
        }
        
        void deallocate(T *p, size_t n)
        {
            HASSERT(n == 1);
            FreeList *pFreeList = (FreeList*)p;
            pFreeList->next = freeList;
            freeList = pFreeList;
        }

        //    size
        size_t max_size() const
        {
            return entries;
        }

        //    construction/destruction

        void construct(T *p, const T& t)
        {
            new(p) T(t);
        }
        
        void destroy(T *p)
        {
            p->~T();
        }

        bool operator==(Allocator const&)
        {
            return true;
        }
        
        bool operator!=(Allocator const& a)
        {
            return !operator==(a);
        }
    private:
    };

    enum
    {
        /** marks an unused mapping */
        UNUSED_MASK = 0x10000000
    };

    struct Metadata
    {
        NodeID id;
        NodeAlias alias;
        long long timestamp;
        Metadata *next;
    };

    /** list of unused mapping entries */
    Metadata *freeList;
    Metadata *metadata;

    RBTree <NodeAlias, Metadata*> aliasTree;
    RBTree <NodeID, Metadata*> idTree;
    RBTree <long long, Metadata*> timeTree;

    RBTree <NodeAlias, Metadata*>::Node *aliasNode;
    RBTree <NodeID, Metadata*>::Node *idNode;
    RBTree <long long, Metadata*>::Node *timeNode;

    Allocator <std::pair<NodeAlias, Metadata*>> aliasAllocator;

    std::map <NodeAlias, Metadata*, nodekey_compare, Allocator<std::pair<NodeAlias, Metadata*>>> *aliasMap;

    //std::map <NodeID, Metadata*, less<NodeID>, Allocator<std::pair<NodeID, Metadata*>>> idMap;

    //std::map <long long, Metadata*, less<long long>, Allocator<std::pair<long long, Metadata*>>> timeMap;

    /** Seed for the generation of the next alias */
    NodeID seed;
    
    /** Update the time stamp for a given entry.
     * @param  metadata metadata associated with the entry */
    inline void touch(Metadata* metadata);

    /** Default Constructor */
    AliasCache();

    /** Default destructor */
    ~AliasCache();
    
    DISALLOW_COPY_AND_ASSIGN(AliasCache);
};

/** Update the time stamp for a given entry.
 * @param  metadata metadata associated with the entry */
inline void AliasCache::touch(Metadata* metadata)
{
    RBTree<long long, Metadata*>::Node *node = timeTree.remove(metadata->timestamp);
    
    HASSERT(node != NULL);
    
    metadata->timestamp = OSTime::get_monotonic();
    
    timeTree.insert(node);
}

};

#endif /* _NMRAnetAliasCache_hxx */
