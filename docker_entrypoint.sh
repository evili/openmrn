#!/bin/bash
set -euo pipefail
git clone ${OPENMRN_URL} .
make -C doc html
exec bash -i
