#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.Thing import Thing
from common import log,const
from world import probability
from common.misc import set_kw
from world.physics.Vector3D import Vector3D

class House(Thing):
    """This base class for houses, building material is wood"""
    def __init__(self, **kw):
    	self.base_init(kw)
    	set_kw(self,kw,"burn_speed",0.2)
        set_kw(self,kw,"material","wood")
        set_kw(self,kw,"weight", 5000.0)
    def tick_operation(self, op):
        """check if we should self-combust
           in any case send ourself next tick"""
        #print `self`,"Got tick operation:\n"
        opTick=Operation("tick",to=self)
        opTick.time.sadd=const.basic_tick
        if probability.does_it_happen(probability.fire_probability):
            fireEntity=Entity(name='fire',type=['fire'],status=0.0,
                              location=Location(self,Vector3D(0.0,0.0,0.0)))
            opCreate=Operation("create",fireEntity,to=self)
            if const.debug_level>=2:
                print "Fire! "*30
            return Message(opCreate,opTick)
        return opTick
