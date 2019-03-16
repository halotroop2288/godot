import os
import platform
import sys
import os.path

def is_active():
    return True

def get_name():
    return "Switch"

def can_build():
    # Check the minimal dependencies
    if not os.path.exists('/opt/devkitpro/devkitA64'):
        print("devkitA64 not found.. switch disabled.")
        return False

    err = os.system("pkg-config --version > /dev/null")
    if err:
        print("pkg-config not found.. switch disabled.")
        return False

    if not os.path.exists("/opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-pkg-config"):
        print("aarch64-none-elf-pkg-config not found.. switch disabled.")
        return False
    return True

def get_opts():
    
    from SCons.Variables import BoolVariable, EnumVariable

    return [
        BoolVariable('use_llvm', 'Use the LLVM compiler', False),
        BoolVariable('use_static_cpp', 'Link libgcc and libstdc++ statically for better portability', False),
        BoolVariable('use_sanitizer', 'Use LLVM compiler address sanitizer', False),
        BoolVariable('use_leak_sanitizer', 'Use LLVM compiler memory leaks sanitizer (implies use_sanitizer)', False),
        BoolVariable('pulseaudio', 'Detect & use pulseaudio', True),
        BoolVariable('udev', 'Use udev for gamepad connection callbacks', False),
        EnumVariable('debug_symbols', 'Add debugging symbols to release builds', 'yes', ('yes', 'no', 'full')),
        BoolVariable('separate_debug_symbols', 'Create a separate file containing debugging symbols', False),
        BoolVariable('touch', 'Enable touch events', True),
        BoolVariable('execinfo', 'Use libexecinfo on systems where glibc is not available', False),
        ]

def get_flags():
    return [
            ('builtin_bullet', False), # Godot needs an unreleased Bullet version (2.88).
            ('builtin_enet', True), # Not in portlibs.
            ('builtin_freetype', False),
            ('builtin_libogg', False),
            ('builtin_libpng', False),
            ('builtin_libtheora', False),
            ('builtin_libvorbis', False),
            ('builtin_libvpx', True), # Not in portlibs.
            ('builtin_libwebp', True), # Not in portlibs.
            ('builtin_libwebsockets', True), # Not in portlibs.
            ('builtin_mbedtls', False), # Hmmmmm, modules/mbedtls/stream_peer_mbed_tls.cpp:32:10: fatal error: mbedtls/platform_util.h: No such file or directory
            ('builtin_miniupnpc', True),
            ('builtin_opus', True), # Portlibs are missing opusfile.
            ('builtin_pcre2', False),
            ('builtin_squish', True), # Not in portlibs.
            ('builtin_zlib', False),
            ('builtin_zstd', True) # Not in portlibs.
        ]


def configure(env):
    env["CC"] = "aarch64-none-elf-gcc"
    env["CXX"] = "aarch64-none-elf-g++"
    env["LD"] = "aarch64-none-elf-ld"
    env["AR"] = "aarch64-none-elf-gcc"

    ## Build type

    dkp = "/opt/devkitpro"
    env['ENV']['DEVKITPRO'] = dkp
    env['ENV']['PATH'] = os.environ['PATH'] + ":{}/portlibs/switch/bin:{}/devkitA64/bin".format(dkp,dkp) # This doesn't quite work....

    arch = ["-march=armv8-a", "-mtune=cortex-a57", "-mtp=soft", "-fPIE"]
    env.Prepend(CCFLAGS=arch + ['-ffunction-sections'])
    env.Prepend(CPPFLAGS='-D__SWITCH__ -I {}/portlibs/switch/include -isystem {}/libnx/include -DPOSH_COMPILER_GCC -DPOSH_OS_HORIZON -DPOSH_OS_STRING=\\"horizon\\"'.format(dkp,dkp).split(" "))
    env.Prepend(LINKFLAGS=arch + ['-specs={}/libnx/switch.specs'.format(dkp), '-L{}/portlibs/switch/lib'.format(dkp), '-L{}/libnx/lib'.format(dkp)])

    if (env["target"] == "release"):
        # -O3 -ffast-math is identical to -Ofast. We need to split it out so we can selectively disable
        # -ffast-math in code for which it generates wrong results.
        if (env["optimize"] == "speed"): #optimize for speed (default)
            env.Prepend(CCFLAGS=['-O3', '-ffast-math'])
        else: #optimize for size
            env.Prepend(CCFLAGS=['-Os'])
     
        if (env["debug_symbols"] == "yes"):
            env.Prepend(CCFLAGS=['-g1'])
        if (env["debug_symbols"] == "full"):
            env.Prepend(CCFLAGS=['-g2'])

    elif (env["target"] == "release_debug"):
        if (env["optimize"] == "speed"): #optimize for speed (default)
            env.Prepend(CCFLAGS=['-O2', '-ffast-math', '-DDEBUG_ENABLED'])
        else: #optimize for size
            env.Prepend(CCFLAGS=['-Os', '-DDEBUG_ENABLED'])

        if (env["debug_symbols"] == "yes"):
            env.Prepend(CCFLAGS=['-g1'])
        if (env["debug_symbols"] == "full"):
            env.Prepend(CCFLAGS=['-g2'])

    elif (env["target"] == "debug"):
        env.Prepend(CCFLAGS=['-g3', '-DDEBUG_ENABLED', '-DDEBUG_MEMORY_ENABLED'])
        #env.Append(LINKFLAGS=['-rdynamic'])

    ## Architecture

    env["bits"] = "64"

    # leak sanitizer requires (address) sanitizer
    if env['use_sanitizer'] or env['use_leak_sanitizer']:
        env.Append(CCFLAGS=['-fsanitize=address', '-fno-omit-frame-pointer'])
        env.Append(LINKFLAGS=['-fsanitize=address'])
        env.extra_suffix += "s"
        if env['use_leak_sanitizer']:
            env.Append(CCFLAGS=['-fsanitize=leak'])
            env.Append(LINKFLAGS=['-fsanitize=leak'])

    if env['use_lto']:
        env.Append(CCFLAGS=['-flto'])
        if env.GetOption("num_jobs") > 1:
            env.Append(LINKFLAGS=['-flto=' + str(env.GetOption("num_jobs"))])
        else:
            env.Append(LINKFLAGS=['-flto'])

        env['RANLIB'] = 'aarch64-none-elf-gcc-ranlib'
        env['AR'] = 'aarch64-none-elf-gcc-ar'
    else:
        env["RANLIB"] = "aarch64-none-elf-ranlib"
        env["AR"] = "aarch64-none-elf-ar"

    env.Append(CCFLAGS=['-pipe'])
    env.Append(LINKFLAGS=['-pipe'])

    ## Dependencies

    if (env['touch']):
        env.Append(CPPFLAGS=['-DTOUCH_ENABLED'])

    # freetype depends on libpng and zlib, so bundling one of them while keeping others
    # as shared libraries leads to weird issues
    if env['builtin_freetype'] or env['builtin_libpng'] or env['builtin_zlib']:
        env['builtin_freetype'] = True
        env['builtin_libpng'] = True
        env['builtin_zlib'] = True

    if not env['builtin_freetype']:
        env.ParseConfig('aarch64-none-elf-pkg-config freetype2 --cflags --libs')

    if not env['builtin_libpng']:
        env.ParseConfig('aarch64-none-elf-pkg-config libpng --cflags --libs')

    if not env['builtin_bullet']:
        # We need at least version 2.88
        import subprocess
        bullet_version = subprocess.check_output(['aarch64-none-elf-pkg-config', 'bullet', '--modversion']).strip()
        if bullet_version < "2.88":
            # Abort as system bullet was requested but too old
            print("Bullet: System version {0} does not match minimal requirements ({1}). Aborting.".format(bullet_version, "2.88"))
            sys.exit(255)
        env.ParseConfig('aarch64-none-elf-pkg-config bullet --cflags --libs')

    if not env['builtin_enet']:
        env.ParseConfig('aarch64-none-elf-pkg-config libenet --cflags --libs')

    if not env['builtin_squish'] and env['tools']:
        env.ParseConfig('aarch64-none-elf-pkg-config libsquish --cflags --libs')

    if not env['builtin_zstd']:
        env.ParseConfig('aarch64-none-elf-pkg-config libzstd --cflags --libs')

    # Sound and video libraries
    # Keep the order as it triggers chained dependencies (ogg needed by others, etc.)

    if not env['builtin_libtheora']:
        env['builtin_libogg'] = False  # Needed to link against system libtheora
        env['builtin_libvorbis'] = False  # Needed to link against system libtheora
        env.ParseConfig('aarch64-none-elf-pkg-config theora theoradec --cflags --libs')
    else:
        list_of_x86 = ['x86_64', 'x86', 'i386', 'i586']
        if any(platform.machine() in s for s in list_of_x86):
            env["x86_libtheora_opt_gcc"] = True

    if not env['builtin_libvpx']:
        env.ParseConfig('aarch64-none-elf-pkg-config vpx --cflags --libs')

    if not env['builtin_libvorbis']:
        env['builtin_libogg'] = False  # Needed to link against system libvorbis
        env.ParseConfig('aarch64-none-elf-pkg-config vorbis vorbisfile --cflags --libs')

    if not env['builtin_opus']:
        env['builtin_libogg'] = False  # Needed to link against system opus
        env.ParseConfig('aarch64-none-elf-pkg-config opus opusfile --cflags --libs')

    if not env['builtin_libogg']:
        env.ParseConfig('aarch64-none-elf-pkg-config ogg --cflags --libs')

    if not env['builtin_libwebp']:
        env.ParseConfig('aarch64-none-elf-pkg-config libwebp --cflags --libs')

    if not env['builtin_mbedtls']:
        # mbedTLS does not provide a pkgconfig config yet. See https://github.com/ARMmbed/mbedtls/issues/228
        env.Append(LIBS=['mbedtls', 'mbedx509', 'mbedcrypto'])

    if not env['builtin_libwebsockets']:
        env.ParseConfig('aarch64-none-elf-pkg-config libwebsockets --cflags --libs')

    if not env['builtin_miniupnpc']:
        # No pkgconfig file so far, hardcode default paths.
        env.Append(CPPPATH=["/usr/include/miniupnpc"])
        env.Append(LIBS=["miniupnpc"])

    # On Linux wchar_t should be 32-bits
    # 16-bit library shouldn't be required due to compiler optimisations
    if not env['builtin_pcre2']:
        env.ParseConfig('aarch64-none-elf-pkg-config libpcre2-32 --cflags --libs')

    ## Flags

    if (os.system("aarch64-none-elf-pkg-config --exists alsa") == 0): # 0 means found
        print("Enabling ALSA")
        env.Append(CPPFLAGS=["-DALSA_ENABLED", "-DALSAMIDI_ENABLED"])
        env.ParseConfig('aarch64-none-elf-pkg-config alsa --cflags --libs')
    else:
        print("ALSA libraries not found, disabling driver")

    if env['pulseaudio']:
        if (os.system("aarch64-none-elf-pkg-config --exists libpulse") == 0): # 0 means found
            print("Enabling PulseAudio")
            env.Append(CPPFLAGS=["-DPULSEAUDIO_ENABLED"])
            env.ParseConfig('aarch64-none-elf-pkg-config --cflags --libs libpulse')
        else:
            print("PulseAudio development libraries not found, disabling driver")

    if (platform.system() == "Linux"):
        env.Append(CPPFLAGS=["-DJOYDEV_ENABLED"])

        if env['udev']:
            if (os.system("aarch64-none-elf-pkg-config --exists libudev") == 0): # 0 means found
                print("Enabling udev support")
                env.Append(CPPFLAGS=["-DUDEV_ENABLED"])
                env.ParseConfig('aarch64-none-elf-pkg-config libudev --cflags --libs')
            else:
                print("libudev development libraries not found, disabling udev support")

    # Linkflags below this line should typically stay the last ones
    #if not env['builtin_zlib']:
    #    env.ParseConfig('aarch64-none-elf-pkg-config zlib --cflags --libs')

    env.Append(CPPPATH=['#platform/switch'])
    env.Append(CPPFLAGS=['-DHORIZON_ENABLED', '-DLIBC_FILEIO_ENABLED', '-DOPENGL_ENABLED', '-DGLES_ENABLED'])
    env.Append(LIBS=['EGL', 'GLESv2', 'glapi', 'drm_nouveau', 'nx'])

    #-lglad -lEGL -lglapi -ldrm_nouveau 
