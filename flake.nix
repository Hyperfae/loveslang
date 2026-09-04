{
  description = "Slang shader compiler wrapper for LÖVE";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/release-26.05";
  };

  outputs = inputs @ {
    nixpkgs,
    self,
    ...
  }: let
      # System types to support.
      supportedSystems = [ "x86_64-linux" ];
      # Helper function to generate an attrset '{ x86_64-linux = f "x86_64-linux"; ... }'.
      forAllSystems = innerSetFunc: (nixpkgs.lib.genAttrs supportedSystems (system: innerSetFunc
        {
          system = system;
          pkgs = import nixpkgs {system = system;};
        }
      ));
  in {
    packages = forAllSystems ({system, pkgs, ...}: {
      loveslang = pkgs.stdenv.mkDerivation (finalAttrs: {
        pname = "loveslang";
        version = "12.0";

        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
        ];

        buildInputs = with pkgs; [
          sdl3
          spirv-tools.lib
          spirv-tools.dev
          freetype
          physfs
          libmodplug
          mpg123
          libvorbis
          libogg
          libtheora
          which
          libtool
          harfbuzz
          openal
          luajit
          curl
          shader-slang
        ];

        meta = {
          # description = "An awesome Lua game framework";
          # homepage = "https://love2d.org/";
          # license = pkgs.lib.licenses.zlib;
          # maintainers = with pkgs.lib.maintainers; [  ];
        };
      });
    });
    devShells = forAllSystems ({system, pkgs, ...}: {
      default = pkgs.mkShell {
        nativeBuildInputs = (self.packages.${system}.loveslang.nativeBuildInputs) ++ (with pkgs; [
          glslang
          gdb
          clang-tools
          spirv-cross
        ]);
        buildInputs = self.packages.${system}.loveslang.buildInputs;
      };
    });
  };
}
