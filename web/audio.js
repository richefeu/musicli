// audio.js — synthèse de corde pincée (Karplus-Strong) avec Web Audio.
//
//   Sound.play(midi)            une note
//   Sound.strum([midi, …])      accord gratté, du grave vers l'aigu
//   Sound.sequence([midi, …])   notes successives
//
// Les échantillons sont calculés une fois par hauteur puis mis en cache.

const Sound = (() => {
  let ctx = null;
  let out = null;
  const cache = new Map();
  let voices = [];

  function ensure() {
    if (!ctx) {
      ctx = new AudioContext();
      const comp = ctx.createDynamicsCompressor();
      out = ctx.createGain();
      out.gain.value = 0.6;
      out.connect(comp).connect(ctx.destination);
    }
    if (ctx.state === "suspended") ctx.resume();
    return ctx;
  }

  const freq = (midi) => 440 * Math.pow(2, (midi - 69) / 12);

  // Karplus-Strong : bruit dans une ligne à retard bouclée sur un filtre moyenneur.
  // Un passe-tout du 1er ordre règle la partie fractionnaire du retard (justesse).
  function pluck(midi) {
    if (cache.has(midi)) return cache.get(midi);
    const sr = ctx.sampleRate;
    const length = Math.floor(sr * 2.5);
    const buffer = ctx.createBuffer(1, length, sr);
    const data = buffer.getChannelData(0);

    const period = sr / freq(midi) - 0.5; // le moyenneur ajoute 1/2 échantillon
    const n = Math.floor(period);
    const frac = period - n;
    const c = (1 - frac) / (1 + frac);
    const decay = 0.996;

    const ring = new Float32Array(n);
    let lp = 0;
    for (let i = 0; i < n; i++) {
      lp = 0.6 * lp + 0.4 * (Math.random() * 2 - 1); // bruit adouci : attaque moins métallique
      ring[i] = lp;
    }

    let idx = 0, prev = 0, apIn = 0, apOut = 0;
    for (let i = 0; i < length; i++) {
      const cur = ring[idx];
      data[i] = cur;
      const avg = decay * 0.5 * (cur + prev);
      prev = cur;
      apOut = c * avg + apIn - c * apOut;
      apIn = avg;
      ring[idx] = apOut;
      idx = idx + 1 === n ? 0 : idx + 1;
    }
    const fade = Math.floor(sr * 0.05);
    for (let i = 0; i < fade; i++) data[length - 1 - i] *= i / fade;

    cache.set(midi, buffer);
    return buffer;
  }

  function voice(midi, when, gain) {
    const src = ctx.createBufferSource();
    src.buffer = pluck(midi);
    const g = ctx.createGain();
    g.gain.value = gain;
    src.connect(g).connect(out);
    src.start(when);
    const v = { src, g };
    voices.push(v);
    src.onended = () => { voices = voices.filter((x) => x !== v); };
  }

  function stop() {
    if (!ctx) return;
    const t = ctx.currentTime;
    for (const { src, g } of voices) {
      g.gain.setTargetAtTime(0, t, 0.02);
      src.stop(t + 0.1);
    }
    voices = [];
  }

  return {
    play(midi) {
      ensure();
      voice(midi, ctx.currentTime, 0.9);
    },
    strum(midis, spacing = 0.035) {
      ensure();
      stop();
      const t = ctx.currentTime + 0.02;
      midis.forEach((m, i) => voice(m, t + i * spacing, 0.7));
    },
    sequence(midis, step = 0.28) {
      ensure();
      stop();
      const t = ctx.currentTime + 0.02;
      midis.forEach((m, i) => voice(m, t + i * step, 0.8));
    },
    stop,
  };
})();
