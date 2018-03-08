import subprocess as s;
import argparse;
import random;
import threading;
from nes import Emulator;

def start_ffmpeg_video(o_path):
    return s.Popen(['ffmpeg', '-y', '-f', 'rawvideo', '-vcodec', 'rawvideo', '-r', '60', '-pix_fmt:v',
        'rgb24', '-video_size', '256x240', '-i', 'pipe:0', '-vcodec', 'h264', '-r', '60', o_path], stdin=s.PIPE)

def start_ffmpeg_audio(o_path):
    return s.Popen(['ffmpeg', '-y', '-f', 'f32le', '-ar', '44100', '-ac', '1', '-i', 'pipe:0', '-acodec', 'mp3', o_path], stdin=s.PIPE)

def load_recording(rom_path, recording_path):
    emu = Emulator(rom_path)
    emu.load_session(recording_path)
    return emu

def merge_video_audio(video_path, audio_path, output_path):
    s.check_call(['ffmpeg', '-y', '-i', video_path, '-i', audio_path, '-vcodec', 'copy', '-acodec', 'copy', output_path])

def delete_file(path):
    s.check_call(['rm', path])

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rom")
    parser.add_argument("--recording")
    parser.add_argument("--output")
    args = parser.parse_args()
    rom_path = args.rom
    recording_path = args.recording
    output_path = args.output
    num = random.randint(0, 1000000000)
    video_path = '/tmp/nes_video_{}.mp4'.format(num)
    audio_path = '/tmp/nes_audio_{}.mp3'.format(num)
    try:
      pv = start_ffmpeg_video(video_path)
      pa = start_ffmpeg_audio(audio_path)
      emu = load_recording(rom_path, recording_path)
      while emu.is_replay():
          emu.emulate()
          pv.stdin.write(emu.get_pixel_values())
          pa.stdin.write(emu.get_audio_samples().tobytes())
      pv.stdin.close()
      pa.stdin.close()
      pv.wait()
      pa.wait()
      merge_video_audio(video_path, audio_path, output_path)
    finally:
      delete_file(video_path)
      delete_file(audio_path)

if __name__ == "__main__":
    main()
