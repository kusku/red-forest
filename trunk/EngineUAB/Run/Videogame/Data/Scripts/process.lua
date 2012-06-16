function change_to_game_process()
	load_data()
	core:get_process():change_process()
end

function change_to_gui_process()
	get_game_process():change_to_gui_process()
	core:get_gui_manager():pop_windows()
	core:get_gui_manager():push_windows('GameOver')
end

function close_game()
	core:get_process():exit_game()
end