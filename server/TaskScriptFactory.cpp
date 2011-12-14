// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include <Python.h>

#include "TaskScriptFactory.h"

#include "rulesets/Py_Task.h"
#include "rulesets/Python_Script_Utils.h"
#include "rulesets/PythonEntityScript.h"

#include "rulesets/Task.h"

/// \brief PythonTaskScriptFactory constructor
///
/// @param package Name of the script package where the script type is
/// @param type Name of the scrpt types instanced by this factory
PythonTaskScriptFactory::PythonTaskScriptFactory(const std::string & package,
                                                 const std::string & type) :
                                                 PythonClass(package, type)
{
}

PythonTaskScriptFactory::~PythonTaskScriptFactory()
{
}

int PythonTaskScriptFactory::setup()
{
    return load();
}

int PythonTaskScriptFactory::check() const
{
    if (!PyType_IsSubtype((PyTypeObject*)m_class, &PyTask_Type)) {
        return -1;
    }
    return 0;
}

const std::string & PythonTaskScriptFactory::package() const
{
    return m_package;
}

int PythonTaskScriptFactory::addScript(Task * entity) const
{
    if (m_class == 0) {
        return -1;
    }
    PyObject * wrapper = wrapPython(entity);
    if (wrapper == 0) {
        return -1;
    }

    PyObject * script = Create_PyScript(wrapper, m_class);

    Py_DECREF(wrapper);

    if (script != NULL) {
        entity->setScript(new PythonEntityScript(script));

        Py_DECREF(script);
    }

    return (script == NULL) ? -1 : 0;
}

int PythonTaskScriptFactory::refreshClass()
{
    return refresh();
}
