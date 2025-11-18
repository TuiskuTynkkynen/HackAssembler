#!/bin/bash

pushd ..

if [ ! -D "./Vendor/premake/bin/release/" ]; then
    pushd ./Vendor/premake/
    ./Bootstrap.sh
    popd
fi

./Vendor/premake/bin/release/premake5 --cc=clang --file=Build.lua gmake2

popd
