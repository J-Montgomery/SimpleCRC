#!/usr/bin/env bash
set -e
! rv-match $@ 2>&1 | grep error