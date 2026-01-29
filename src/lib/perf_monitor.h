#pragma once

#include <Arduino.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * Performance Monitor for PixelFrame
 * Tracks CPU usage, frame times, API response times, and memory usage
 */

class PerfMonitor {
public:
    static PerfMonitor* Instance() {
        if (instance == nullptr) {
            instance = new PerfMonitor();
        }
        return instance;
    }

    // Frame timing
    void startFrameTimer() {
        frameStartMicros = micros();
    }

    void endFrameTimer() {
        uint32_t elapsed = micros() - frameStartMicros;
        frameTimeSum += elapsed;
        frameCount++;
        
        if (elapsed > frameTimeMax) frameTimeMax = elapsed;
        if (elapsed < frameTimeMin || frameTimeMin == 0) frameTimeMin = elapsed;
    }

    // API timing
    void startApiTimer() {
        apiStartMicros = micros();
    }

    void endApiTimer() {
        uint32_t elapsed = micros() - apiStartMicros;
        apiTimeSum += elapsed;
        apiCallCount++;
        
        if (elapsed > apiTimeMax) apiTimeMax = elapsed;
    }

    // WebSocket timing
    void recordWebSocketEvent() {
        wsEventCount++;
    }

    // Track frame drops (when frame time exceeds target)
    void checkFrameDrop(uint32_t targetMs = 12) {
        uint32_t elapsed = micros() - frameStartMicros;
        if (elapsed > targetMs * 1000) {
            frameDropCount++;
        }
    }

    // Memory tracking
    uint32_t getFreeHeap() {
        return ESP.getFreeHeap();
    }

    uint32_t getMinFreeHeap() {
        return ESP.getMinFreeHeap();
    }

    uint32_t getLargestFreeBlock() {
        return ESP.getMaxAllocHeap();
    }

    // CPU tracking
    float getCpuUsageCore0() {
        // Note: This is an approximation based on idle time
        return cpuUsageCore0;
    }

    float getCpuUsageCore1() {
        return cpuUsageCore1;
    }

    // Print stats to serial
    void printStats() {
        if (frameCount == 0) return;
        
        uint32_t avgFrameTime = frameTimeSum / frameCount;
        float fps = frameCount > 0 ? 1000000.0f / avgFrameTime : 0;
        
        Serial.println(F("\n========== PERFORMANCE STATS =========="));
        
        // Frame stats
        Serial.println(F("--- Frame Rendering ---"));
        Serial.printf("  Avg frame time: %lu µs (%.1f FPS)\n", avgFrameTime, fps);
        Serial.printf("  Min/Max frame: %lu / %lu µs\n", frameTimeMin, frameTimeMax);
        Serial.printf("  Frame count: %lu\n", frameCount);
        Serial.printf("  Frame drops: %lu (%.1f%%)\n", frameDropCount, 
            frameCount > 0 ? (frameDropCount * 100.0f / frameCount) : 0);
        
        // API stats
        if (apiCallCount > 0) {
            Serial.println(F("--- API Calls ---"));
            Serial.printf("  API calls: %lu\n", apiCallCount);
            Serial.printf("  Avg API time: %lu µs\n", apiTimeSum / apiCallCount);
            Serial.printf("  Max API time: %lu µs\n", apiTimeMax);
            Serial.printf("  WebSocket events: %lu\n", wsEventCount);
        }
        
        // Memory stats
        Serial.println(F("--- Memory ---"));
        Serial.printf("  Free heap: %lu bytes\n", getFreeHeap());
        Serial.printf("  Min free heap: %lu bytes\n", getMinFreeHeap());
        Serial.printf("  Largest block: %lu bytes\n", getLargestFreeBlock());
        Serial.printf("  Heap fragmentation: %.1f%%\n", 
            100.0f - (getLargestFreeBlock() * 100.0f / getFreeHeap()));
        
        // Task stats
        Serial.println(F("--- Tasks ---"));
        Serial.printf("  Running core: %d\n", xPortGetCoreID());
        Serial.printf("  Free stack (current task): %lu bytes\n", 
            uxTaskGetStackHighWaterMark(NULL) * 4);
        
        Serial.println(F("========================================\n"));
    }

    // Reset stats (call periodically)
    void reset() {
        frameTimeSum = 0;
        frameTimeMin = 0;
        frameTimeMax = 0;
        frameCount = 0;
        frameDropCount = 0;
        apiTimeSum = 0;
        apiTimeMax = 0;
        apiCallCount = 0;
        wsEventCount = 0;
    }

    // JSON output for API endpoint
    String toJson() {
        uint32_t avgFrameTime = frameCount > 0 ? frameTimeSum / frameCount : 0;
        float fps = frameCount > 0 ? 1000000.0f / avgFrameTime : 0;
        
        String json = "{";
        json += "\"frame\":{";
        json += "\"avgTimeUs\":" + String(avgFrameTime) + ",";
        json += "\"minTimeUs\":" + String(frameTimeMin) + ",";
        json += "\"maxTimeUs\":" + String(frameTimeMax) + ",";
        json += "\"fps\":" + String(fps, 1) + ",";
        json += "\"count\":" + String(frameCount) + ",";
        json += "\"drops\":" + String(frameDropCount);
        json += "},";
        json += "\"api\":{";
        json += "\"calls\":" + String(apiCallCount) + ",";
        json += "\"avgTimeUs\":" + String(apiCallCount > 0 ? apiTimeSum / apiCallCount : 0) + ",";
        json += "\"maxTimeUs\":" + String(apiTimeMax) + ",";
        json += "\"wsEvents\":" + String(wsEventCount);
        json += "},";
        json += "\"memory\":{";
        json += "\"freeHeap\":" + String(getFreeHeap()) + ",";
        json += "\"minFreeHeap\":" + String(getMinFreeHeap()) + ",";
        json += "\"largestBlock\":" + String(getLargestFreeBlock());
        json += "},";
        json += "\"core\":" + String(xPortGetCoreID());
        json += "}";
        return json;
    }

private:
    PerfMonitor() {
        reset();
    }

    static PerfMonitor* instance;

    // Frame timing
    uint32_t frameStartMicros = 0;
    uint32_t frameTimeSum = 0;
    uint32_t frameTimeMin = 0;
    uint32_t frameTimeMax = 0;
    uint32_t frameCount = 0;
    uint32_t frameDropCount = 0;

    // API timing
    uint32_t apiStartMicros = 0;
    uint32_t apiTimeSum = 0;
    uint32_t apiTimeMax = 0;
    uint32_t apiCallCount = 0;
    uint32_t wsEventCount = 0;

    // CPU usage (approximation)
    float cpuUsageCore0 = 0;
    float cpuUsageCore1 = 0;
};

// Singleton instance
inline PerfMonitor* PerfMonitor::instance = nullptr;

// Convenience macros for easy instrumentation
#define PERF_FRAME_START() PerfMonitor::Instance()->startFrameTimer()
#define PERF_FRAME_END() PerfMonitor::Instance()->endFrameTimer()
#define PERF_FRAME_CHECK() PerfMonitor::Instance()->checkFrameDrop()
#define PERF_API_START() PerfMonitor::Instance()->startApiTimer()
#define PERF_API_END() PerfMonitor::Instance()->endApiTimer()
#define PERF_WS_EVENT() PerfMonitor::Instance()->recordWebSocketEvent()
#define PERF_PRINT() PerfMonitor::Instance()->printStats()
#define PERF_RESET() PerfMonitor::Instance()->reset()
#define PERF_JSON() PerfMonitor::Instance()->toJson()
