import asyncio
import os
from concurrent.futures import ThreadPoolExecutor
from typing import Callable, Any



class TagWorker:
    """Process tagging jobs concurrently using a worker pool."""

    def __init__(self, *, max_workers: int | None = None, loop: asyncio.AbstractEventLoop | None = None):
        self.loop = loop or asyncio.get_event_loop()
        self.max_workers = max_workers or os.cpu_count() or 1
        self.queue: asyncio.Queue[tuple[str, Callable[[str], Any], Callable[[Any], None]]] = asyncio.Queue()
        self.executor = ThreadPoolExecutor(max_workers=self.max_workers)
        self.workers = [self.loop.create_task(self._worker()) for _ in range(self.max_workers)]

    def enqueue(self, path: str, handler: Callable[[str], Any], callback: Callable[[Any], None]) -> None:
        """Add a job to the queue."""
        self.queue.put_nowait((path, handler, callback))

    async def _worker(self) -> None:
        while True:
            path, handler, callback = await self.queue.get()
            try:
                result = await self.loop.run_in_executor(self.executor, handler, path)
                callback(result)
            finally:
                self.queue.task_done()
