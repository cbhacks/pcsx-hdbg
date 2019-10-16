#
# PCSX-HDBG - PCSX-based hack debugger
# Copyright (C) 2019  "chekwob" <chek@wobbyworks.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

set -e

case "${BUILD}" in
    "")
        echo '$BUILD was not set!'
        exit 1
        ;;
    "linux")
        sudo apt-get update -qq
        sudo apt-get install -qq libsdl2-dev
        (
            mkdir -p build
            cd build
            cmake ..
            make
        )
        ;;
    "mingw")
        sudo apt-get update -qq
        sudo apt-get install -qq p7zip-full
        (
            mkdir -p build
            cd build
            ../tools/mingw-cross-build.sh
        )
        DEPLOYMENT_FILE=$( \
            printf \
            'pcsx-hdbg-n%04d-%s-%s.7z' \
            "${TRAVIS_BUILD_NUMBER}" \
            "${TRAVIS_COMMIT}" \
            "${ARCH}" \
        )
        mv build/pcsx-hdbg.7z "${DEPLOYMENT_FILE}"
        ;;
    *)
        echo 'Unrecognized $BUILD option: '"${BUILD}"
        exit 1
        ;;
esac

(
    # Only deploy for selected builds.
    if [ -z "${DEPLOY}" ]; then
        echo 'No artifacts will be deployed; build was not marked for deployment'
        exit
    fi

    # Don't deploy if there is nothing to deploy.
    if [ -z "${DEPLOYMENT_FILE}" ]; then
        echo 'No artifacts will be deployed; no files'
        exit
    fi

    # Don't deploy for pull requests (it will fail, no S3 keys).
    if [ "${TRAVIS_PULL_REQUEST}" != "false" ]; then
        echo 'No artifacts will be deployed; this is a pull request'
        exit
    fi

    # Only deploy for master branch
    if [ "${TRAVIS_BRANCH}" != "master" ]; then
        echo 'No artifacts will be deployed; not on a deployable branch'
        exit
    fi

    wget 'https://s3.amazonaws.com/travis-ci-gmbh/artifacts/stable/build/linux/amd64/artifacts'
    chmod +x artifacts
    ./artifacts upload "${DEPLOYMENT_FILE}"
)
