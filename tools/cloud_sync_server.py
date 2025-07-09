#!/usr/bin/env python3
"""Simple cloud sync server storing playback status per user."""

from flask import Flask, request, jsonify
import sys

app = Flask(__name__)
_status = {}

@app.route('/status', methods=['GET', 'POST'])
def status():
    if request.method == 'POST':
        data = request.get_json(force=True) or {}
        user = data.get('user')
        if not user:
            return 'user required', 400
        _status[user] = {
            'path': data.get('path', ''),
            'position': data.get('position', 0.0),
        }
        return '', 200
    user = request.args.get('user')
    info = _status.get(user, {'path': '', 'position': 0.0})
    return jsonify(info)

if __name__ == '__main__':
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 56793
    app.run(host='0.0.0.0', port=port)
