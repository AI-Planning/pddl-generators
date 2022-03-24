#! /bin/bash

set -euo pipefail

REVISION="HEAD"
NAME="pddl-generators"
ZIPFILE="$NAME.zip"

git archive --worktree-attributes --format zip --prefix "$NAME/" -o "$ZIPFILE" "$REVISION"

echo "Zipfile: file://$(realpath $ZIPFILE)"
