// lib/control_switches.dart

import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';

class ControlSwitches extends StatelessWidget {
  final bool manualOverride;
  final bool manualToggle;
  final DatabaseReference podRef;

  const ControlSwitches({
    super.key,
    required this.manualOverride,
    required this.manualToggle,
    required this.podRef,
  });

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        width: double.infinity, // ← растягиваем
        padding: const EdgeInsets.all(16),
        decoration: BoxDecoration(
          color: Colors.white,
          borderRadius: BorderRadius.circular(16),
          boxShadow: [
            BoxShadow(
              color: Colors.black.withOpacity(0.1),
              blurRadius: 10,
              offset: const Offset(0, 4),
            ),
          ],
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            const Text('Light control', style: TextStyle(fontSize: 16)),
            const SizedBox(height: 16),

            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Text(
                  manualOverride ? 'Mode: Manual' : 'Mode: Auto',
                  style: const TextStyle(fontSize: 16),
                ),
                Switch(
                  value: manualOverride,
                  activeColor: Colors.orange,
                  onChanged: (bool value) {
                    podRef
                        .child('settings/light_mode/manual_override')
                        .set(value ? 1 : 0);
                  },
                ),
              ],
            ),

            const SizedBox(height: 20),

            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Text(
                  manualToggle ? 'Light: On' : 'Light: Off',
                  style: const TextStyle(fontSize: 16),
                ),
                Switch(
                  value: manualToggle,
                  activeColor: Colors.green,
                  onChanged: (bool value) {
                    podRef
                        .child('settings/light_mode/manual_toggle')
                        .set(value ? 1 : 0);
                  },
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
