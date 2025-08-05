#!/usr/bin/env python3
"""
GameEngine Cross-Platform Build Helper
Supports Windows, macOS, and Linux with multiple build systems
"""

import os
import sys
import subprocess
import platform
import argparse
from pathlib import Path

def detect_platform():
    """Detect the current platform"""
    system = platform.system().lower()
    if system == "windows":
        return "windows"
    elif system == "darwin":
        return "macos"
    elif system == "linux":
        return "linux"
    else:
        return "unknown"

def check_dependencies():
    """Check if required dependencies are available"""
    deps = {
        "g++": "C++ compiler",
        "make": "Make build system"
    }
    
    missing = []
    for cmd, desc in deps.items():
        try:
            subprocess.run([cmd, "--version"], capture_output=True, check=True)
        except (subprocess.CalledProcessError, FileNotFoundError):
            missing.append(f"{cmd} ({desc})")
    
    return missing

def check_sfml():
    """Check if SFML is available"""
    platform_name = detect_platform()
    
    if platform_name == "macos":
        # Check Homebrew SFML
        homebrew_paths = [
            "/opt/homebrew/opt/sfml@2",
            "/opt/homebrew/opt/sfml",
            "/usr/local/opt/sfml@2",
            "/usr/local/opt/sfml"
        ]
        for path in homebrew_paths:
            if os.path.exists(f"{path}/include/SFML"):
                return True, f"Found at {path}"
    
    elif platform_name == "linux":
        # Check pkg-config
        try:
            subprocess.run(["pkg-config", "--exists", "sfml-all"], check=True)
            return True, "Found via pkg-config"
        except (subprocess.CalledProcessError, FileNotFoundError):
            pass
        
        # Check system paths
        system_paths = ["/usr/include/SFML", "/usr/local/include/SFML"]
        for path in system_paths:
            if os.path.exists(path):
                return True, f"Found at {path}"
    
    elif platform_name == "windows":
        # Check common Windows paths
        windows_paths = [
            "C:/SFML",
            "C:/msys64/mingw64/include/SFML"
        ]
        for path in windows_paths:
            if os.path.exists(path):
                return True, f"Found at {path}"
    
    return False, "Not found"

def build_makefile(debug=False, clean=False):
    """Build using Makefile"""
    print("Building with Makefile...")
    
    if clean:
        subprocess.run(["make", "clean"])
    
    cmd = ["make"]
    if debug:
        cmd.append("debug")
    
    result = subprocess.run(cmd)
    return result.returncode == 0

def build_cmake(debug=False, clean=False):
    """Build using CMake"""
    print("Building with CMake...")
    
    build_dir = Path("cmake_build")
    
    if clean and build_dir.exists():
        import shutil
        shutil.rmtree(build_dir)
    
    build_dir.mkdir(exist_ok=True)
    os.chdir(build_dir)
    
    # Configure
    cmake_cmd = ["cmake", ".."]
    if debug:
        cmake_cmd.extend(["-DCMAKE_BUILD_TYPE=Debug"])
    else:
        cmake_cmd.extend(["-DCMAKE_BUILD_TYPE=Release"])
    
    result = subprocess.run(cmake_cmd)
    if result.returncode != 0:
        return False
    
    # Build
    result = subprocess.run(["make"])
    os.chdir("..")
    
    return result.returncode == 0

def run_game():
    """Run the built game"""
    executables = [
        "build/release/GameEngine",
        "build/debug/GameEngine",
        "cmake_build/bin/GameEngine",
        "build.out"
    ]
    
    for exe in executables:
        if os.path.exists(exe):
            print(f"Running {exe}...")
            subprocess.run([f"./{exe}"])
            return True
    
    print("No executable found. Please build first.")
    return False

def show_status():
    """Show build system status"""
    platform_name = detect_platform()
    missing_deps = check_dependencies()
    sfml_available, sfml_info = check_sfml()
    
    print("GameEngine Build System Status")
    print("=" * 40)
    print(f"Platform: {platform_name.title()}")
    print(f"SFML: {'✓' if sfml_available else '✗'} {sfml_info}")
    
    if missing_deps:
        print(f"Missing dependencies: {', '.join(missing_deps)}")
    else:
        print("Dependencies: ✓ All required tools available")
    
    # Check for existing builds
    builds = []
    if os.path.exists("build/release/GameEngine"):
        builds.append("Makefile (Release)")
    if os.path.exists("build/debug/GameEngine"):
        builds.append("Makefile (Debug)")
    if os.path.exists("cmake_build/bin/GameEngine"):
        builds.append("CMake")
    if os.path.exists("build.out"):
        builds.append("Legacy")
    
    if builds:
        print(f"Available builds: {', '.join(builds)}")
    else:
        print("Available builds: None")

def show_help():
    """Show platform-specific help"""
    platform_name = detect_platform()
    
    print("GameEngine Build Help")
    print("=" * 40)
    
    if platform_name == "windows":
        print("Windows Setup:")
        print("1. Install MSYS2 from https://www.msys2.org/")
        print("2. Run: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sfml")
        print("3. Add MSYS2 to PATH")
        print("4. Run: python build.py --build")
    
    elif platform_name == "macos":
        print("macOS Setup:")
        print("1. Install Homebrew from https://brew.sh/")
        print("2. Run: brew install sfml@2")
        print("3. Run: python build.py --build")
    
    elif platform_name == "linux":
        print("Linux Setup:")
        print("Ubuntu/Debian: sudo apt-get install build-essential libsfml-dev")
        print("CentOS/RHEL: sudo yum install gcc-c++ SFML-devel")
        print("Arch: sudo pacman -S gcc sfml")
        print("Then run: python build.py --build")

def main():
    parser = argparse.ArgumentParser(description="GameEngine Cross-Platform Build Helper")
    parser.add_argument("--build", choices=["make", "cmake"], default="make",
                       help="Build system to use (default: make)")
    parser.add_argument("--debug", action="store_true", help="Build in debug mode")
    parser.add_argument("--clean", action="store_true", help="Clean before building")
    parser.add_argument("--run", action="store_true", help="Run the game after building")
    parser.add_argument("--status", action="store_true", help="Show build system status")
    parser.add_argument("--help-setup", action="store_true", help="Show platform-specific setup help")
    
    args = parser.parse_args()
    
    if args.help_setup:
        show_help()
        return
    
    if args.status:
        show_status()
        return
    
    # Check dependencies
    missing_deps = check_dependencies()
    if missing_deps:
        print(f"Error: Missing dependencies: {', '.join(missing_deps)}")
        print("Run with --help-setup for installation instructions")
        return
    
    # Check SFML
    sfml_available, sfml_info = check_sfml()
    if not sfml_available:
        print(f"Error: SFML not found ({sfml_info})")
        print("Run with --help-setup for installation instructions")
        return
    
    # Build
    success = False
    if args.build == "cmake":
        success = build_cmake(args.debug, args.clean)
    else:
        success = build_makefile(args.debug, args.clean)
    
    if not success:
        print("Build failed!")
        return
    
    print("Build successful!")
    
    # Run if requested
    if args.run:
        run_game()

if __name__ == "__main__":
    main()
