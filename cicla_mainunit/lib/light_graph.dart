// lib/light_graph.dart

import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';

class LightGraph extends StatelessWidget {
  final List<FlSpot> avgLightPoints;
  final List<FlSpot> brightnessPoints;
  final List<FlSpot> thresholdPoints;

  const LightGraph({
    super.key,
    required this.avgLightPoints,
    required this.brightnessPoints,
    required this.thresholdPoints,
  });

  @override
  Widget build(BuildContext context) {
    if (avgLightPoints.isEmpty) {
      return const SizedBox(
        height: 300,
        child: Center(child: Text('Waiting for data...')),
      );
    }

    return SizedBox(
      height: 300,
      child: LineChart(
        LineChartData(
          gridData: const FlGridData(show: true),
          titlesData: const FlTitlesData(show: false),
          borderData: FlBorderData(show: true),
          lineTouchData: const LineTouchData(enabled: false),
          minY: 0,
          maxY: 1,
          lineBarsData: [
            LineChartBarData(
              spots: avgLightPoints,
              isCurved: false,
              color: Colors.blue,
              barWidth: 3,
              dotData: const FlDotData(show: false),
            ),
            LineChartBarData(
              spots: brightnessPoints,
              isCurved: false,
              color: Colors.green,
              barWidth: 3,
              dotData: const FlDotData(show: false),
            ),
            LineChartBarData(
              spots: thresholdPoints,
              isCurved: false,
              color: Colors.red,
              barWidth: 3,
              dotData: const FlDotData(show: false),
            ),
          ],
        ),
      ),
    );
  }
}
