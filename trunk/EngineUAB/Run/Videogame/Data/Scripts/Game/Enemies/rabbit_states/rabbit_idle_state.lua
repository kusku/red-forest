class 'CRabbitIdleState' (CState)
	function CRabbitIdleState:__init() 
		CState.__init(self)
		print_logger(0, "Inicio del estado idle de la caperucita")
	end

	function CRabbitIdleState:OnEnter(_CCharacter)
		-- print_logger(0, "CRabbitIdleState:Enter")
		if not ( _CCharacter == nil ) then
			num = _CCharacter:get_animation_id("idle")
			_CCharacter:get_animation_model():blend_cycle( num, 0.3 )
		end
	end
	
	function CRabbitIdleState:Execute(_CCharacter)
		--print_logger(0, "CRabbitIdleState:Execute")
		-- local l_distance = get_distance_to_player(_CCharacter, _CCharacter.player)
		-- print_logger (1, "Distancia al player: "..l_distance)
		if ( is_player_detected( _CCharacter, _CCharacter.player ) ) then 
			-- Is Attacable --
			-- if ( is_player_attackable( _CCharacter, _CCharacter.player ) ) then
				-- --_CCharacter.graphic_fsm:change_state(_CCharacter.attack_state)
				
			-- -- Pursuit --
			-- else
				-- player no atacable i lo perseguimos pq lo hemos detectado
				
				-- 1) Caso en que est� lej�simo. Seguramente debe hacer un rodeo y usar los waypoints
				-- if ( ( l_distance > _CCharacter.properties.distance_chase ) and ( l_distance <= _CCharacter.properties.distance_path_follow ) ) then
					-- _CCharacter.logic_fsm:change_state(_CCharacter.path_follow_state)
					-- print_logger (1, "ara molt lluny")
				-- 2) Caso en que est� cerca y lo detecto pero no demasiado
				--elseif ( ( l_distance <= _CCharacter.properties.detection_distance ) and ( l_distance > _CCharacter.properties.distance_chase ) ) then
					_CCharacter:move_to( _CCharacter.player.position, _CCharacter.elapsed_time)
					_CCharacter.logic_fsm:change_state(_CCharacter.pursuit_state)
					--print_logger (1, "ara not tant lluny")
				-- 3) Caso en que est� cerquisimo y voy a por �l
				-- elseif ( l_distance < _CCharacter.properties.distance_chase ) then
					-- _CCharacter.logic_fsm:change_state(_CCharacter.seek_state)
					-- print_logger (1, "ara molt aprop")
				-- end 
			-- end
		
		-- No detecto player --> no hago nada o patrullo. TODO!!
		else
			-- print_logger(0 , "posici�n :".._CCharacter.position.x.." ".._CCharacter.position.y.." ".._CCharacter.position.z)
			_CCharacter:move_to( _CCharacter.position, _CCharacter.elapsed_time)
		end
	end
	
	function CRabbitIdleState:OnExit(_CCharacter)
		--print_logger(0, "CRabbitIdleState:Exit")
		if not ( _CCharacter == nil ) then
			num = _CCharacter:get_animation_id("idle")
			_CCharacter:get_animation_model():clear_cycle( num, 0.3 )
		end
	end
	
	function CRabbitIdleState:OnMessage(_CCharacter, _Msg)
		print_logger(0, "CRabbitIdleState:OnMessage")	
		if ( _Msg.msg == msg_attack ) then
			_CCharacter.logic_fsm:change_state(_CCharacter.hit_state)
			return true
		end
		return false
	end
	
	function CRabbitIdleState:__Finalize()
	
	end
	