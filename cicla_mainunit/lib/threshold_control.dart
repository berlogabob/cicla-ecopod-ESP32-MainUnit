// lib/threshold_control.dart

import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';

class ThresholdControl extends StatefulWidget {
  final int currentValue;
  final DatabaseReference podRef;

  const ThresholdControl({
    super.key,
    required this.currentValue,
    required this.podRef,
  });

  @override
  State<ThresholdControl> createState() => _ThresholdControlState();
}

class _ThresholdControlState extends State<ThresholdControl> {
  late double value;

  @override
  void initState() {
    super.initState();
    int rounded = ((widget.currentValue / 100).round() * 100).clamp(100, 4000);
    value = rounded.toDouble();
  }

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
            const Text(
              'Light threshold control',
              style: TextStyle(fontSize: 16),
            ),
            const SizedBox(height: 16),
            Slider(
              min: 100,
              max: 4000,
              divisions: 39,
              value: value,
              label: value.round().toString(),
              onChanged: (double newValue) {
                setState(() {
                  value = newValue;
                });
                widget.podRef
                    .child('settings/light_mapping/light_threshold')
                    .set(newValue.round());
              },
            ),
            Text(
              'Current: ${value.round()}',
              style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 18),
            ),
          ],
        ),
      ),
    );
  }
}
