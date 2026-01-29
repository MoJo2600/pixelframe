/**
 * Custom middleware for PixelFrame mock API
 * Simulates ESP32 behavior for testing
 */

// Track current frame state
let currentState = {
  mode: 'clock',
  gif: null,
  visual: null,
  brightness: 164
};

// Simulated performance data that changes over time
let perfData = {
  frameCount: 0,
  apiCalls: 0,
  startTime: Date.now()
};

module.exports = (req, res, next) => {
  // Add CORS headers
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Headers', '*');
  res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, PATCH, DELETE, OPTIONS');

  // Handle OPTIONS preflight
  if (req.method === 'OPTIONS') {
    return res.sendStatus(200);
  }

  // Track API calls
  perfData.apiCalls++;

  // Handle show/clock
  if (req.path === '/api/show/clock') {
    currentState.mode = 'clock';
    console.log('[MOCK] Switching to clock mode');
    return res.json({ message: 'Switching to clock', mode: 'clock' });
  }

  // Handle show/off
  if (req.path === '/api/show/off') {
    currentState.mode = 'off';
    console.log('[MOCK] Switching to off mode');
    return res.json({ message: 'Switching to off', mode: 'off' });
  }

  // Handle show/gif
  if (req.path === '/api/show/gif') {
    currentState.mode = 'gif';
    currentState.gif = req.query.image || 'random';
    console.log(`[MOCK] Switching to gif: ${currentState.gif}`);
    return res.json({ 
      message: 'Switching to gif', 
      mode: 'gif',
      image: currentState.gif,
      duration: req.query.duration || 10
    });
  }

  // Handle show/visuals
  if (req.path === '/api/show/visuals') {
    currentState.mode = 'visuals';
    currentState.visual = req.query.v || 'random';
    console.log(`[MOCK] Switching to visual: ${currentState.visual}`);
    return res.json({ 
      message: 'Switching to visuals', 
      mode: 'visuals',
      visual: currentState.visual
    });
  }

  // Handle performance endpoint with dynamic data
  if (req.path === '/api/perf') {
    const uptime = Date.now() - perfData.startTime;
    perfData.frameCount += Math.floor(Math.random() * 1000) + 500;
    
    return res.json({
      frame: {
        avgTimeUs: 1200 + Math.floor(Math.random() * 200),
        minTimeUs: 800 + Math.floor(Math.random() * 100),
        maxTimeUs: 3000 + Math.floor(Math.random() * 2000),
        fps: 78 + Math.random() * 4,
        count: perfData.frameCount,
        drops: Math.floor(perfData.frameCount * 0.0005)
      },
      api: {
        calls: perfData.apiCalls,
        avgTimeUs: 2500 + Math.floor(Math.random() * 500),
        maxTimeUs: 10000 + Math.floor(Math.random() * 10000),
        wsEvents: Math.floor(perfData.apiCalls * 0.3)
      },
      memory: {
        freeHeap: 180000 + Math.floor(Math.random() * 10000),
        minFreeHeap: 160000 + Math.floor(Math.random() * 5000),
        largestBlock: 100000 + Math.floor(Math.random() * 20000)
      },
      core: 1,
      mode: currentState.mode,
      uptime: uptime
    });
  }

  // Handle perf reset
  if (req.path === '/api/perf/reset') {
    perfData.frameCount = 0;
    perfData.apiCalls = 0;
    perfData.startTime = Date.now();
    console.log('[MOCK] Performance stats reset');
    return res.send('Performance stats reset');
  }

  // Handle perf print
  if (req.path === '/api/perf/print') {
    console.log('[MOCK] Performance stats printed to console');
    return res.send('Stats printed to serial');
  }

  // Handle heap
  if (req.path === '/heap') {
    return res.send(String(180000 + Math.floor(Math.random() * 10000)));
  }

  // Handle status endpoint (custom)
  if (req.path === '/api/status') {
    return res.json({
      mode: currentState.mode,
      gif: currentState.gif,
      visual: currentState.visual,
      brightness: currentState.brightness,
      uptime: Date.now() - perfData.startTime
    });
  }

  // Add slight delay to simulate network latency (50-150ms)
  const delay = 50 + Math.floor(Math.random() * 100);
  setTimeout(() => next(), delay);
};
