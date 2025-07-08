import os
import time
import psutil
import requests
import sqlite3

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


DB = 'media_library.db'


def check_db(path):
    if not os.path.exists(DB):
        return 0
    with sqlite3.connect(DB) as conn:
        cur = conn.execute('SELECT COUNT(*) FROM MediaTags WHERE path=?', (path,))
        row = cur.fetchone()
        return row[0] if row else 0


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
    print('tags in db', check_db(AUDIO))
    profile(call_video, VIDEO)
    print('tags in db', check_db(VIDEO))


if __name__ == '__main__':
    main()
