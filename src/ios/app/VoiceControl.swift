import Foundation
import Speech
import AVFoundation

class VoiceControl: NSObject {
    private let audioEngine = AVAudioEngine()
    private var request: SFSpeechAudioBufferRecognitionRequest?
    private var task: SFSpeechRecognitionTask?
    private let recognizer = SFSpeechRecognizer()

    var onCommand: ((String) -> Void)?

    func start() {
        SFSpeechRecognizer.requestAuthorization { auth in
            guard auth == .authorized else { return }
            DispatchQueue.main.async {
                self.beginRecognition()
            }
        }
    }

    private func beginRecognition() {
        stop()
        request = SFSpeechAudioBufferRecognitionRequest()
        guard let request = request else { return }
        let node = audioEngine.inputNode
        let format = node.outputFormat(forBus: 0)
        node.installTap(onBus: 0, bufferSize: 1024, format: format) { buffer, _ in
            request.append(buffer)
        }
        task = recognizer?.recognitionTask(with: request) { result, error in
            if let text = result?.bestTranscription.formattedString {
                self.onCommand?(text)
            }
            if result?.isFinal == true || error != nil {
                self.stop()
            }
        }
        audioEngine.prepare()
        try? audioEngine.start()
    }

    func stop() {
        audioEngine.stop()
        audioEngine.inputNode.removeTap(onBus: 0)
        request?.endAudio()
        task?.cancel()
        request = nil
        task = nil
    }
}
