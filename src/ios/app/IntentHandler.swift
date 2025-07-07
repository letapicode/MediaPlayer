import Intents

class IntentHandler: INExtension, INPlayMediaIntentHandling, INPauseMediaIntentHandling {
    private let player = MediaPlayerViewModel()

    func handle(intent: INPlayMediaIntent, completion: @escaping (INPlayMediaIntentResponse) -> Void) {
        player.play()
        completion(INPlayMediaIntentResponse(code: .success, userActivity: nil))
    }

    func handle(intent: INPauseMediaIntent, completion: @escaping (INPauseMediaIntentResponse) -> Void) {
        player.pause()
        completion(INPauseMediaIntentResponse(code: .success, userActivity: nil))
    }
}
