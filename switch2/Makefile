build-romfs:
	pnpm run build

build-nro: build-romfs
	pnpm run nro


debug: build-nro
	/opt/devkitpro/tools/bin/nxlink PKSM.nro