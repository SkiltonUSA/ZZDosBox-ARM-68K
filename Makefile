PYTHON ?= python3
.PHONY: verify prepare test-host test-model
verify:
	$(PYTHON) scripts/verify_backup.py
prepare: verify
	$(PYTHON) scripts/restore_build_inputs.py
test-host:
	mkdir -p .context/tests
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/launcher tests/amiga/dosbox/launcher.c -o .context/tests/launcher
	.context/tests/launcher
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/mesen_nes tests/amiga/dosbox/present_plan.c -o .context/tests/present
	.context/tests/present
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/mesen_nes tests/amiga/dosbox/direct_plan.c -o .context/tests/direct
	.context/tests/direct
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/mesen_nes tests/amiga/dosbox/profile.c -o .context/tests/profile
	.context/tests/profile
	clang++ -std=c++17 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/mesen_nes tests/amiga/dosbox/capture.cpp -o .context/tests/capture
	.context/tests/capture
	clang++ -std=c++17 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/mesen_nes tests/amiga/dosbox/scale.cpp -o .context/tests/scale
	.context/tests/scale
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc tests/amiga/dosbox/mouse_geometry.c -o .context/tests/mouse
	.context/tests/mouse
test-model:
	$(PYTHON) tests/amiga/dosbox/check_pc_arm.py
