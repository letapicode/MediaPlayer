import asyncio
from typing import Callable, Any
from collections import deque


class TagWorker:
    def __init__(self, loop: asyncio.AbstractEventLoop | None = None):
        self.loop = loop or asyncio.get_event_loop()
        self.queue: deque[tuple[str, Callable[[Any], None], Callable[[str], Any]]] = deque()
        self.running = False

    def enqueue(self, path: str, handler: Callable[[str], Any], callback: Callable[[Any], None]):
        self.queue.append((path, callback, handler))
        if not self.running:
            self.loop.create_task(self._run())
            self.running = True

    async def _run(self):
        while self.queue:
            path, callback, handler = self.queue.popleft()
            result = await self.loop.run_in_executor(None, handler, path)
            callback(result)
        self.running = False
