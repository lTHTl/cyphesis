#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#return Operation("create",Entity(name='wood',type=['lumber'],location=self.location.parent.location.copy()),to=self)

from atlas import *

from world.objects.Thing import Thing
from common import set_kw

class Sword(Thing):
    """This is base class for swords, this one just ordinary sword"""
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"weight",4)
    def cut_operation(self, op):
        to_ = self.world.get_object(op[1].id)
        if not to_:
            return self.error(op,"To is undefined object")
        return Operation("touch",op[1],to=to_,from_=self)
