PYTHON ?= python3
.PHONY: verify prepare test-host test-model
verify:
	$(PYTHON) scripts/verify_backup.py
prepare: verify
	$(PYTHON) scripts/restore_build_inputs.py
test-host:
	mkdir -p .context/tests
	clang++ -std=c++17 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc tests/amiga/dosbox/descent_clock.cpp -o .context/tests/descent-clock
	.context/tests/descent-clock
	$(CXX) -std=c++17 -O2 -fsanitize=address,undefined -Iamiga/dosbox/pc tests/amiga/dosbox/duke_clock.cpp -o .context/tests/duke-clock
	.context/tests/duke-clock
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/launcher tests/amiga/dosbox/launcher.c -o .context/tests/launcher
	.context/tests/launcher
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/common tests/amiga/dosbox/present_plan.c -o .context/tests/present
	.context/tests/present
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/common tests/amiga/dosbox/direct_plan.c -o .context/tests/direct
	.context/tests/direct
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/common tests/amiga/dosbox/profile.c -o .context/tests/profile
	.context/tests/profile
	clang++ -std=c++17 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/common tests/amiga/dosbox/capture.cpp -o .context/tests/capture
	.context/tests/capture
	clang++ -std=c++17 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc -Iamiga/common tests/amiga/dosbox/scale.cpp -o .context/tests/scale
	.context/tests/scale
	clang -std=c99 -O2 -Wall -Wextra -Werror -Iamiga/dosbox/pc tests/amiga/dosbox/mouse_geometry.c -o .context/tests/mouse
	.context/tests/mouse
	clang++ -std=c++17 -O2 -fsanitize=address,undefined -Iamiga/dosbox/pc tests/amiga/dosbox/scanline_plan.cpp -o .context/tests/scanlines
	.context/tests/scanlines
	clang -std=c99 -O2 -fsanitize=address,undefined -Iamiga/dosbox/pc tests/amiga/dosbox/dynrec_plan.c -o .context/tests/dynrec
	.context/tests/dynrec
test-model:
	$(PYTHON) tests/amiga/dosbox/check_pc_arm.py

.PHONY: build-current
build-current:
	$(PYTHON) scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
	  --pc-neon --pc-pipeline --pc-scanlines --pc-presentation --pc-adapters \
	  --pc-compat --pc-x87 --pc-file-readahead --pc-pic-vfp

.PHONY: test-network
test-network:
	$(PYTHON) scripts/test_dosbox_network.py
	$(PYTHON) scripts/test_dosbox_developer.py

.PHONY: test-host-review
test-host-review:
	$(PYTHON) scripts/test_dosbox_host_review.py

.PHONY: build-expanded build-network-jit test-file-read-ahead
build-expanded:
	$(PYTHON) scripts/build_dosbox_dos_arm.py --pc --pc-compat --pc-x87 --pc-expanded \
	  --pc-dynrec --pc-jit-mib 4 --pc-neon --pc-pipeline --pc-pic-vfp \
	  --pc-scanlines --pc-presentation --pc-adapters --pc-file-readahead
build-network-jit:
	$(PYTHON) scripts/build_dosbox_dos_arm.py --pc --pc-compat --pc-x87 --pc-network \
	  --pc-dynrec --pc-jit-mib 4 --pc-neon --pc-pipeline --pc-pic-vfp \
	  --pc-disk-cache-kib 1024 --pc-file-readahead
test-file-read-ahead:
	$(PYTHON) scripts/test_dosbox_file_read_ahead.py
