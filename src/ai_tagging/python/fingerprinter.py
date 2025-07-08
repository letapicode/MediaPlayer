import os
from typing import Optional

import acoustid

API_KEY = os.environ.get("ACOUSTID_API_KEY")


def fingerprint(path: str) -> Optional[str]:
    """Return a MusicBrainz ID if recognized, otherwise None."""
    if not API_KEY:
        return None
    try:
        results = acoustid.match(API_KEY, path)
    except acoustid.AcoustidError:
        return None
    for score, rid, title, artist in results:
        if score > 0.5:
            return rid
    return None
