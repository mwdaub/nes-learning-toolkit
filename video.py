import subprocess as s;
import argparse;
import random;
from nes import Emulator;

def create_fifo(path):
    s.call(["mkfifo", path])

def delete_fifo(path):
    s.call(['rm', path])

def start_ffmpeg(v_path, a_path, o_path):
    p = s.Popen(['ffmpeg', '-y', '-f', 'rawvideo', '-vcodec', 'rawvideo', '-r', '60', '-pix_fmt:v', 'rgb24', '-video_size', '256x240', '-i', v_path, '-vcodec', 'mpeg4', '-r', '60', o_path])
    return p

def start_replay(rom_path, i_path, v_path, a_path):
    emu = Emulator(rom_path)
    emu.replay_session(i_path, v_path, a_path)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rom")
    parser.add_argument("--recording")
    parser.add_argument("--output")
    args = parser.parse_args()
    rom_path = args.rom
    i_path = args.recording
    o_path = args.output
    num = random.randint(0, 1000000000)
    v_path = '/tmp/nes_video_{}'.format(num)
    # a_path = '/tmp/nes_audio_{}'.format(num)
    a_path = '/dev/null'
    create_fifo(v_path)
    # create_fifo(a_path)
    try:
        p = start_ffmpeg(v_path, a_path, o_path)
        start_replay(rom_path, i_path, v_path, a_path)
        p.wait()
    except:
        p.terminate()
    finally:
        delete_fifo(v_path)
        # delete_fifo(a_path)


if __name__ == "__main__":
    main()
