import Foundation
import CoreMotion

class ShakeDetector {
    private let motion = CMMotionManager()
    var onShake: (() -> Void)?

    func start() {
        guard motion.isAccelerometerAvailable else { return }
        motion.accelerometerUpdateInterval = 0.2
        motion.startAccelerometerUpdates(to: .main) { [weak self] data, _ in
            guard let data = data else { return }
            let acc = data.acceleration
            let magnitude = sqrt(acc.x * acc.x + acc.y * acc.y + acc.z * acc.z)
            if magnitude > 2.7 {
                self?.onShake?()
            }
        }
    }

    func stop() {
        motion.stopAccelerometerUpdates()
    }
}
