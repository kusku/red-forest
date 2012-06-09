-- ------------------------------------------------------------------------
--	Clase CEnemy
-- ------------------------------------------------------------------------
class 'CEnemy' (CCharacter)
	function CEnemy:__init(id)
		CCharacter.__init(self, id)
		
		self.yaw = 0.0
		self.pitch = -math.pi / 8
		self.roll = 0.0
		self.position = Vect3f(0.0, 0.0, 0.0)
		self.locked = false
		
		--self:__create_states()
	end

	function CEnemy:__init(id, name)
		CCharacter.__init(self, id, name)
		
		self.yaw = 0.0
		self.pitch = -math.pi / 8
		self.roll = 0.0
		self.position = Vect3f(0.0, 0.0, 0.0)
		self.locked = false
		
		self.animation_idle_state = CWolfIdleState()
	end
	
	function CEnemy:init()
		l_fsm =	self.get_graphic_fsm 
		if (l_fsm == nil) then
			print_logger(2, "CEnemy:init()->No se ha podido obtener la m�quina de estados.")
			return false
		else
			l_fsm.current_state = self.animation_idle_state 
		end
		
		return true
	end
	
	function CEnemy:update(elapsed_time)
		print_logger(0, "hola")
		self.physic_controller:move(Vect3f(0.0,0.0,0.0), elapsed_time)
		
		--Actualiza la posici�n del objeto 3D
		self.position = self.physic_controller.position
		--self.position = Vect3f(self.position.x, self.position.y - self.physic_controller.height + 0.4, self.position.z)
		
		--Actualiza la posici�n del modelo animado
		self.animated_model.position = self.position
		
		local l_Fsm = self.get_graphic_fsm 
		l_Fsm:change_state(self.animation_idle_state)
	end
