# coding:utf-8
#!/usr/bin/python
#
# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#
# -- This line is 75 characters -------------------------------------------
# The __init__.py files help guide import statements without automatically
# importing all of the modules
"""azpy.maya.__init__"""

from azpy.env_bool import env_bool
from azpy.constants import ENVAR_DCCSI_DEV_MODE

#  global space
_DCCSI_DEV_MODE = env_bool(ENVAR_DCCSI_DEV_MODE, False)

_PACKAGENAME = __name__
if _PACKAGENAME is '__main__':
    _PACKAGENAME = 'azpy.maya'

import azpy
_LOGGER = azpy.initialize_logger(_PACKAGENAME)
_LOGGER.debug('Invoking __init__.py for {0}.'.format({_PACKAGENAME}))

# -------------------------------------------------------------------------

__all__ = []

# -------------------------------------------------------------------------


# -------------------------------------------------------------------------
if _DCCSI_DEV_MODE:
    # If in dev mode this will test imports of __all__
    from azpy import test_imports
    _LOGGER.debug('Testing Imports from {0}'.format(_PACKAGENAME))
    test_imports(__all__,
                 _pkg=_PACKAGENAME,
                 _logger=_LOGGER)
# -------------------------------------------------------------------------


# -------------------------------------------------------------------------
def init():
    """If the maya api is required for a package/module to import,
    then it should be initialized and added here so general imports
    don't fail"""

    # Make sure we can import the native apis
    import maya.cmds as mc
    import maya.api.OpenMaya as om
    
    __all__.append('callbacks')
    __all__.append('helpers')
    __all__.append('toolbits')

    # Importing local packages/modules
    pass
# -------------------------------------------------------------------------


# -------------------------------------------------------------------------
if _DCCSI_DEV_MODE:
    # If in dev mode this will test imports of __all__
    from azpy import test_imports
    _LOGGER.debug('Testing Imports from {0}'.format(_PACKAGENAME))
    test_imports(__all__,
                 _pkg=_PACKAGENAME,
                 _logger=_LOGGER)
# -------------------------------------------------------------------------

del _LOGGER
