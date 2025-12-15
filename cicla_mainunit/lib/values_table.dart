// lib/values_table.dart

import 'package:flutter/material.dart';

class ValuesTable extends StatelessWidget {
  final int avgLight;
  final int brightness;

  const ValuesTable({
    super.key,
    required this.avgLight,
    required this.brightness,
  });

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        width: 300, // ← фиксированная ширина
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
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            _row('avg_light:', '$avgLight'),
            const Divider(height: 16, thickness: 1),
            _row('brightness:', '$brightness'),
          ],
        ),
      ),
    );
  }

  Widget _row(String label, String value) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceBetween,
      children: [
        Text(label),
        const SizedBox(width: 32),
        Text(value, style: const TextStyle(fontWeight: FontWeight.bold)),
      ],
    );
  }
}
