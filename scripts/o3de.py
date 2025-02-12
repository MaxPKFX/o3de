#
# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#

import argparse
import pathlib
import sys


def add_args(parser, subparsers) -> None:
    """
    add_args is called to add expected parser arguments and subparsers arguments to each command such that it can be
    invoked by o3de.py
    Ex o3de.py can invoke the register  downloadable commands by importing register,
    call add_args and execute: python o3de.py register --gem-path "C:/TestGem"
    :param parser: the caller instantiates a parser and passes it in here
    :param subparsers: the caller instantiates subparsers and passes it in here
    """

    # As o3de.py shares the same name as the o3de package attempting to use a regular
    # from o3de import <module> line tries to import from the current o3de.py script and not the package
    # So the {current script directory} / 'o3de' is added to the front of the sys.path
    script_dir = pathlib.Path(__file__).parent
    o3de_package_dir = (script_dir / 'o3de').resolve()
    # add the scripts/o3de directory to the front of the sys.path
    sys.path.insert(0, str(o3de_package_dir))
    from o3de import engine_template, global_project, register, print_registration, get_registration, \
        enable_gem, disable_gem, project_properties, sha256
    # Remove the temporarily added path
    sys.path = sys.path[1:]

    # global_project
    global_project.add_args(subparsers)
    # engine templaate
    engine_template.add_args(subparsers)

    # register
    register.add_args(subparsers)

    # show
    print_registration.add_args(subparsers)

    # get-registered
    get_registration.add_args(subparsers)

    # add a gem to a project
    enable_gem.add_args(subparsers)

    # remove a gem from a project
    disable_gem.add_args(subparsers)
    
    # modify project properties 
    project_properties.add_args(subparsers)
    
    # sha256
    sha256.add_args(subparsers)


if __name__ == "__main__":
    # parse the command line args
    the_parser = argparse.ArgumentParser()

    # add subparsers
    the_subparsers = the_parser.add_subparsers(help='sub-command help')

    # add args to the parser
    add_args(the_parser, the_subparsers)

    # parse args
    the_args = the_parser.parse_args()

    # if empty print help
    if len(sys.argv) == 1:
        the_parser.print_help(sys.stderr)
        sys.exit(1)

    # run
    ret = the_args.func(the_args)

    # return
    sys.exit(ret)
