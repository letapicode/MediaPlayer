import os
import time
import psutil
import requests

AUDIO = os.path.join('tests', 'sample_media', 'sample.wav')
VIDEO = os.path.join('tests', 'sample_media', 'sample.mp4')


def profile(func, path):
    start = time.time()
    proc = psutil.Process()
    mem_before = proc.memory_info().rss
    result = func(path)
    duration = time.time() - start
    mem_after = proc.memory_info().rss
    print(f'{func.__name__} took {duration:.2f}s, memory diff {(mem_after - mem_before)/1e6:.2f} MB')
    print(result)


def wait_result(job_id):
    while True:
        resp = requests.get(f'http://localhost:8000/result/{job_id}')
        data = resp.json()
        if data.get('status') != 'pending':
            return data
        time.sleep(0.5)


def main():
    def call_audio(_):
        with open(AUDIO, 'rb') as f:
            resp = requests.post('http://localhost:8000/tag/audio', files={'file': f})
        job_id = resp.json()['job_id']
        return wait_result(job_id)

    def call_video(_):
        with open(VIDEO, 'rb') as f:
            resp = requests.post('http://localhost:8000/tag/video', files={'file': f})
        job_id = resp.json()['job_id']
        return wait_result(job_id)

    profile(call_audio, AUDIO)
    profile(call_video, VIDEO)


if __name__ == '__main__':
    main()
