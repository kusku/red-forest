class 'CPlayerAnimationAttackState' (CState)
	function CPlayerAnimationAttackState:__init() 
		CState.__init(self)
	end

	function CPlayerAnimationAttackState:OnEnter(_CCharacter)
		if not ( _CCharacter == nil ) then
			num = _CCharacter:get_animation_id("attack1")
			_CCharacter:get_animation_model():execute_action( num, 0.3 )
		end
	end
	
	function CPlayerAnimationAttackState:Execute(_CCharacter)
	end
	
	function CPlayerAnimationAttackState:OnExit(_CCharacter)
	end
	
	function CPlayerAnimationAttackState:OnMessage(_CCharacter, _Msg)
		return false
	end
	
	function CPlayerAnimationAttackState:__Finalize()
	end