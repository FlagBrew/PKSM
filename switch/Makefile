build-romfs:
	pnpm run build

build-nro: build-romfs
	pnpm run nro

build-nsp: build-romfs
	pnpm run nsp

debug: build-nro
	/opt/devkitpro/tools/bin/nxlink PKSM.nro

