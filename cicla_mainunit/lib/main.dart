import 'dart:async';

import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';

import 'pod_title.dart';
import 'values_table.dart';
import 'graph_legend.dart';
import 'light_graph.dart';
import 'control_switches.dart';
import 'threshold_control.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();

  await Firebase.initializeApp(
    options: const FirebaseOptions(
      apiKey: "any",
      appId: "1:1234567890:web:abcdef",
      messagingSenderId: "1234567890",
      projectId: "booking-ee47f",
      databaseURL:
          "https://booking-ee47f-default-rtdb.europe-west1.firebasedatabase.app",
    ),
  );

  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      debugShowCheckedModeBanner: false,
      home: MonitoringPage(),
    );
  }
}

class MonitoringPage extends StatefulWidget {
  const MonitoringPage({super.key});

  @override
  State<MonitoringPage> createState() => _MonitoringPageState();
}

class _MonitoringPageState extends State<MonitoringPage> {
  late DatabaseReference podRef;

  int avgLight = 0;
  int brightness = 0;
  int lightThreshold = 1400;

  bool manualOverride = false;
  bool manualToggle = false;

  final List<FlSpot> avgLightPoints = [];
  final List<FlSpot> brightnessPoints = [];
  final List<FlSpot> thresholdPoints = [];

  double startTime = DateTime.now().millisecondsSinceEpoch / 1000.0;

  Timer? plotTimer;

  @override
  void initState() {
    super.initState();

    podRef = FirebaseDatabase.instance.ref('devices/Pod_01_base_01');

    plotTimer = Timer.periodic(const Duration(milliseconds: 400), (_) {
      double now = DateTime.now().millisecondsSinceEpoch / 1000.0;
      double x = now - startTime;

      avgLightPoints.add(FlSpot(x, avgLight / 4095.0));
      brightnessPoints.add(FlSpot(x, brightness / 255.0));
      thresholdPoints.add(FlSpot(x, lightThreshold / 4095.0));

      if (avgLightPoints.length > 100) {
        avgLightPoints.removeAt(0);
        brightnessPoints.removeAt(0);
        thresholdPoints.removeAt(0);
      }

      setState(() {});
    });

    podRef.onValue.listen((event) {
      if (event.snapshot.value == null) return;

      final data = event.snapshot.value as Map<Object?, Object?>;

      setState(() {
        avgLight = data['avg_light'] as int? ?? 0;
        brightness = data['brightness'] as int? ?? 0;

        final settings = data['settings'] as Map<Object?, Object?>;
        final lightMap = settings['light_mapping'] as Map<Object?, Object?>;
        lightThreshold = lightMap['light_threshold'] as int? ?? 1400;

        final lightMode = settings['light_mode'] as Map<Object?, Object?>;
        manualOverride = (lightMode['manual_override'] as int? ?? 0) == 1;
        manualToggle = (lightMode['manual_toggle'] as int? ?? 0) == 1;
      });
    });
  }

  @override
  void dispose() {
    plotTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    double screenWidth = MediaQuery.of(context).size.width;
    double blockWidth = screenWidth * 0.9;
    if (blockWidth > 350) blockWidth = 350;

    return Scaffold(
      appBar: AppBar(title: const Text('Pod Monitoring'), centerTitle: true),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            const PodTitle(),
            const SizedBox(height: 10),

            SizedBox(
              width: blockWidth,
              child: ValuesTable(avgLight: avgLight, brightness: brightness),
            ),

            const SizedBox(height: 10),

            SizedBox(
              width: blockWidth,
              child: ThresholdControl(
                currentValue: lightThreshold,
                podRef: podRef,
              ),
            ),

            const SizedBox(height: 10),

            SizedBox(
              width: blockWidth,
              child: ControlSwitches(
                manualOverride: manualOverride,
                manualToggle: manualToggle,
                podRef: podRef,
              ),
            ),

            const SizedBox(height: 10),

            const GraphLegend(),
            const SizedBox(height: 10),

            const Text(
              'Light values over time',
              style: TextStyle(fontSize: 18),
            ),
            const SizedBox(height: 10),

            LightGraph(
              avgLightPoints: avgLightPoints,
              brightnessPoints: brightnessPoints,
              thresholdPoints: thresholdPoints,
            ),

            const SizedBox(height: 40),
          ],
        ),
      ),
    );
  }
}
