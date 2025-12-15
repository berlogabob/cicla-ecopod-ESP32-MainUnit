import 'package:flutter/material.dart';

class GraphLegend extends StatelessWidget {
  const GraphLegend({super.key});

  @override
  Widget build(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: const [
        LegendItem(color: Colors.blue, text: 'avg_light'),
        SizedBox(width: 20),
        LegendItem(color: Colors.green, text: 'brightness'),
        SizedBox(width: 20),
        LegendItem(color: Colors.red, text: 'light_threshold'),
      ],
    );
  }
}

class LegendItem extends StatelessWidget {
  final Color color;
  final String text;

  const LegendItem({required this.color, required this.text, super.key});

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Container(width: 20, height: 4, color: color),
        const SizedBox(width: 8),
        Text(text),
      ],
    );
  }
}
