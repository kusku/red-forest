class 'CWolfEvadeState' (CState)
	function CWolfEvadeState:__init() 
		CState.__init(self)
		print_logger(0, "Inicio del estado run de la caperucita")
	end

	function CWolfEvadeState:OnEnter(_CCharacter)
		--print_logger(0, "CWolfEvadeState:Enter")
		if not ( _CCharacter == nil ) then
		end
	end
	
	function CWolfEvadeState:Execute(_CCharacter)
		--print_logger(0, "CWolfEvadeState:Execute")
	end
	
	function CWolfEvadeState:OnExit(_CCharacter)
		--print_logger(0, "CWolfEvadeState:Exit")
		if not ( _CCharacter == nil ) then
		end
	end
	
	function CWolfEvadeState:OnMessage(_CCharacter, _Msg)
		print_logger(0, "CWolfEvadeState:OnMessage")
		return false
	end
	
	function CWolfEvadeState:__Finalize()
	
	end