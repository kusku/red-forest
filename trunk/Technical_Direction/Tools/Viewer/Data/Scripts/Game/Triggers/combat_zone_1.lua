-- loadfile "utils.lua"

class 'CCombatZone1Trigger' (CBoxTrigger)
	function CCombatZone1Trigger:__init()
		CBoxTrigger.__init(self)

		self.action_time = CActionStateCallback(0,0.1)
		self.action_time:init_action()
		
		self.enable_action_time = CActionStateCallback(0,0.2)
		self.enable_action_time:init_action()
		
		self.enemy_appeared = 1
		self.total_enemies  = 3 	-- Total de enemigos en la lista
		self.is_appeared = false
		self.first_time_entered = false -- Nos indica si es la primera vez que entra para evitar que vuelva a cargar enemigos y pete lua
	end
	
	function CCombatZone1Trigger:update(elapsed_time)
		-- print_logger(0, "CCombatZone1Trigger:update -> Actualizacion trigger zona de combate")
		
		local t = { "enemy2", "enemy3", "enemy4" }
		
		if ( self.action_time:is_action_started() ) then
			if ( self.action_time:is_action_finished() ) then
				-- Ahora inicializo la parte de la aparición
				self.enable_action_time:init_action()
				self.enable_action_time:start_action()
				self.is_appeared = false
			else 
				self.action_time:update(elapsed_time)
				
				local l_enemy = get_game_process():get_character_manager():get_enemy_by_name(t[self.enemy_appeared])
				if ( l_enemy.enable == false ) then
					l_enemy:appearance()
					self.is_appeared = true
					-- self.enable_action_time:init_action()
					-- self.enable_action_time:start_action()
				end
			end 
		end

		if ( self.enable_action_time:is_action_started() ) then
			if ( self.enable_action_time:is_action_finished() ) then
				--if ( get_game_process():get_character_manager():get_enemy_by_name(t[self.enemy_appeared]).enable == false ) then
					get_game_process():get_character_manager():get_enemy_by_name(t[self.enemy_appeared]).enable = true
					
					if (self.enemy_appeared ~= self.total_enemies ) then
						self.enemy_appeared = self.enemy_appeared + 1
						self.action_time:init_action()
						self.action_time:start_action()
					end 
				-- end
			else 
				self.enable_action_time:update(elapsed_time)
			end 
		end

		if ( self:are_enemies_dead() == true ) then
			-- print_logger(0, "CCombatZone1Trigger:update -> Eliminamos el fuego")
		
			core:get_limit_manager():activate_group("zona1", false)
			_soundM:play_event('zona1_wall001','Stop_EFX_fire_forest_inferno')
			_soundM:play_event('zona1_wall002','Stop_EFX_fire_forest_inferno')
			_soundM:play_event('zona1_wall003','Stop_EFX_fire_forest_inferno')
			self.active = false
		end
	end
	
	function CCombatZone1Trigger:on_enter(user_data)
		if ( self.first_time_entered == false ) then 	
		
			local process = get_game_process()
			local l_player_user_data = process:get_character_manager():get_player().physic_controller.user_data
			
			if( core:get_physics_manager():compare_user_data(user_data, l_player_user_data) ) then
				self.action_time:start_action()
				self.enemy_appeared = 1
				self.is_appeared = false
			end 

			core:get_limit_manager():activate_group("zona1", true)
			_soundM:play_event('zona1_wall001','Play_EFX_fire_forest_inferno')
			_soundM:play_event('zona1_wall002','Play_EFX_fire_forest_inferno')
			_soundM:play_event('zona1_wall003','Play_EFX_fire_forest_inferno')
			self.first_time_entered = true
		end 
	end
	
	function CCombatZone1Trigger:on_stay(user_data)
	end
	
	function CCombatZone1Trigger:on_exit(user_data)
	end
	
	function CCombatZone1Trigger:are_enemies_dead()
		local l_previous_enemies_to_die = { "enemy2", "enemy3", "enemy4" }
		local l_total_enemies_to_die = 3
		
		local l_are_dead = false
		
		for i=1,l_total_enemies_to_die do 
			local l_enemy = get_game_process():get_character_manager():get_enemy_by_name(l_previous_enemies_to_die[i])
			if ( l_enemy:is_alive() ) then
				-- print_logger(0, "CCombatZone1Trigger:update -> Enemigo vivo")
		
				l_are_dead = false
				break
			else 
				-- print_logger(0, "CCombatZone1Trigger:update -> Enemigo muerto")
		
				l_are_dead = true
			end  	
		end 
		return l_are_dead
	end