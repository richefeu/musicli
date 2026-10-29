// app.js — interface web de MusiCLI. Tout le calcul musical vient du module
// WebAssembly (bindings.cpp) ; ce fichier ne fait que l'état, le rendu et le son.

const SHARPS = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
const FLATS  = ["C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"];
const SVG_NS = "http://www.w3.org/2000/svg";
const STORAGE_KEY = "musicli.state";

const FAMILIES = [
  { id: "root",    label: "Fondamentale", test: (d) => d === "1" },
  { id: "third",   label: "Tierce",       test: (d) => d[0] === "3" },
  { id: "fifth",   label: "Quinte",       test: (d) => d[0] === "5" },
  { id: "seventh", label: "Septième",     test: (d) => d[0] === "7" },
  { id: "other",   label: "Autre degré",  test: () => true },
];
const familyOf = (degree) => FAMILIES.find((f) => f.test(degree)).id;
const colorOf = (degree) => `var(--${familyOf(degree)})`;

let M = null; // module WebAssembly
const catalog = { scale: [], chord: [] };

const state = {
  mode: "scale",
  key: { scale: "major", chord: "major" },
  rootPC: 9,
  acc: "sharp",
  tuning: 0,
  frets: 15,
  labels: "notes",
  showAll: false,
};
let view = null;      // résultat du dernier calcul
let diagram = -1;     // diagramme sélectionné
let filter = "";

const $ = (id) => document.getElementById(id);

// ── Persistance (confort uniquement) ──────────────────────────────────────

function loadState() {
  try {
    const saved = JSON.parse(localStorage.getItem(STORAGE_KEY) || "null");
    if (saved) Object.assign(state, saved, { key: { ...state.key, ...saved.key } });
  } catch (e) { /* stockage indisponible : valeurs par défaut */ }
}

function saveState() {
  try { localStorage.setItem(STORAGE_KEY, JSON.stringify(state)); } catch (e) { /* ignoré */ }
}

// ── Calcul ────────────────────────────────────────────────────────────────

const rootName = () => (state.acc === "flat" ? FLATS : SHARPS)[state.rootPC];
const currentEntry = () =>
  catalog[state.mode].find((e) => e.key === state.key[state.mode]) || catalog[state.mode][0];

function compute() {
  const entry = currentEntry();
  const root = rootName();
  const v = { entry, root, notes: [], degrees: [], grid: null, diagrams: [], error: null };
  try {
    const set = state.mode === "scale" ? M.scaleNotes(entry.key, root) : M.chordNotes(entry.key, root);
    v.notes = set.notes;
    v.degrees = set.degrees;
    if (state.mode === "chord") v.diagrams = M.chordDiagrams(state.tuning, entry.key, root, 4);
  } catch (e) {
    v.error = `Impossible de calculer « ${entry.name} » (formule ${entry.formula}) : ${e.message}`;
  }
  v.grid = M.fretboard(state.tuning, state.frets, root, v.notes, v.degrees);
  return v;
}

// Hauteurs ascendantes à partir de la fondamentale (octave 3), octave comprise.
function ascendingMidi(notes) {
  const pcs = notes.map((n) => view.grid.flat().find((c) => c.name === n)?.pc ?? 0);
  const out = [];
  let prev = 48 + pcs[0] - 1;
  for (const pc of pcs) {
    let m = prev - (prev % 12) + pc;
    while (m <= prev) m += 12;
    out.push(m);
    prev = m;
  }
  return out;
}

// ── Rendu ─────────────────────────────────────────────────────────────────

function svg(tag, attrs = {}, parent) {
  const el = document.createElementNS(SVG_NS, tag);
  for (const [k, v] of Object.entries(attrs)) el.setAttribute(k, v);
  if (parent) parent.appendChild(el);
  return el;
}

function renderControls() {
  document.querySelectorAll("[data-mode]").forEach((b) =>
    b.classList.toggle("on", b.dataset.mode === state.mode));
  document.querySelectorAll("[data-labels]").forEach((b) =>
    b.classList.toggle("on", b.dataset.labels === state.labels));
  document.querySelectorAll("[data-acc]").forEach((b) =>
    b.classList.toggle("on", b.dataset.acc === state.acc));
  $("tuning").value = state.tuning;
  $("frets").value = state.frets;
  $("showAll").checked = state.showAll;

  const names = state.acc === "flat" ? FLATS : SHARPS;
  const roots = $("roots");
  roots.replaceChildren(...names.map((n, pc) => {
    const b = document.createElement("button");
    b.textContent = n;
    b.setAttribute("role", "radio");
    b.setAttribute("aria-checked", pc === state.rootPC);
    b.classList.toggle("on", pc === state.rootPC);
    b.onclick = () => { state.rootPC = pc; update(); };
    return b;
  }));
}

function filteredEntries() {
  const q = filter.trim().toLowerCase();
  if (!q) return catalog[state.mode];
  return catalog[state.mode].filter((e) =>
    [e.key, e.name, e.description, ...e.aliases].some((s) => s.toLowerCase().includes(q)));
}

function renderList() {
  const list = $("list");
  const current = state.key[state.mode];
  list.replaceChildren(...filteredEntries().map((e) => {
    const b = document.createElement("button");
    b.className = "item" + (e.key === current ? " on" : "");
    b.setAttribute("role", "option");
    b.setAttribute("aria-selected", e.key === current);
    b.tabIndex = -1;
    b.innerHTML = `${e.name}<small>${e.formula}</small>`;
    b.onclick = () => { state.key[state.mode] = e.key; update(); };
    return b;
  }));
  list.querySelector(".on")?.scrollIntoView({ block: "nearest" });
}

function renderSummary() {
  const { entry, root } = view;
  $("title").textContent = `${root} ${entry.name}`;
  const symbols = state.mode === "chord" && entry.aliases.length
    ? ` · Symboles : ${entry.aliases.join(", ")}` : "";
  $("subtitle").textContent = `Formule ${entry.formula} · ${entry.description}${symbols}`;

  $("error").hidden = !view.error;
  $("error").textContent = view.error || "";

  $("notes").replaceChildren(...view.notes.map((n, i) => {
    const b = document.createElement("button");
    b.className = "note-chip";
    b.style.setProperty("--c", colorOf(view.degrees[i]));
    b.innerHTML = `<b>${n}</b><span>${view.degrees[i]}</span>`;
    b.onclick = () => Sound.play(ascendingMidi(view.notes)[i]);
    return b;
  }));
}

function renderNeck() {
  const grid = view.grid;
  const tuning = catalog.tunings[state.tuning];
  const F = state.frets;
  const nStr = grid.length;

  const W = 1100, LEFT = 30, OPEN = 40, RIGHT = 14, TOP = 18, GAP = 28;
  const nut = LEFT + OPEN;
  const bottomString = TOP + (nStr - 1) * GAP;
  const H = bottomString + 44;
  const span = W - nut - RIGHT;

  // Espacement des frettes : mélange de l'espacement réel (2^(-f/12)) et uniforme,
  // pour garder des cases aiguës lisibles.
  const realF = 1 - Math.pow(2, -F / 12);
  const fretX = (f) => nut + span * (0.55 * (1 - Math.pow(2, -f / 12)) / realF + 0.45 * f / F);
  const cellX = (f) => (f === 0 ? LEFT + OPEN / 2 : (fretX(f - 1) + fretX(f)) / 2);
  const stringY = (s) => TOP + (nStr - 1 - s) * GAP; // corde aiguë en haut

  const el = $("neck");
  el.replaceChildren();
  el.setAttribute("viewBox", `0 0 ${W} ${H}`);

  svg("rect", { class: "board", x: nut, y: TOP - 12, width: fretX(F) - nut, height: bottomString - TOP + 24, rx: 3 }, el);

  const midY = (TOP + bottomString) / 2;
  for (let f = 1; f <= F; f++) {
    const m = f % 12;
    const x = cellX(f);
    if (m === 0) {
      svg("circle", { class: "inlay", cx: x, cy: midY - GAP, r: 6 }, el);
      svg("circle", { class: "inlay", cx: x, cy: midY + GAP, r: 6 }, el);
    } else if ([3, 5, 7, 9].includes(m)) {
      svg("circle", { class: "inlay", cx: x, cy: midY, r: 6 }, el);
    }
    svg("line", { class: "fret", x1: fretX(f), x2: fretX(f), y1: TOP - 12, y2: bottomString + 12 }, el);
    svg("text", { class: "fretno", x, y: bottomString + 32 }, el).textContent = f;
  }
  svg("line", { class: "nut", x1: nut, x2: nut, y1: TOP - 12, y2: bottomString + 12 }, el);

  for (let s = 0; s < nStr; s++) {
    const y = stringY(s);
    svg("line", { class: "string", x1: LEFT + 6, x2: fretX(F), y1: y, y2: y, "stroke-width": 1 + (nStr - 1 - s) * 0.3 }, el);
    svg("text", { class: "strname", x: 12, y }, el).textContent = tuning.strings[s];
  }

  // Position du diagramme sélectionné : corde → case
  const shape = diagram >= 0 ? view.diagrams[diagram]?.frets : null;

  for (let s = 0; s < nStr; s++) {
    for (let f = 0; f <= F; f++) {
      const c = grid[s][f];
      const x = cellX(f), y = stringY(s);
      const g = svg("g", { class: "cell" }, el);
      const w = f === 0 ? OPEN : fretX(f) - fretX(f - 1);
      svg("rect", { class: "hit", x: x - w / 2, y: y - GAP / 2, width: w, height: GAP }, g);
      svg("title", {}, g).textContent = `${c.name}${c.degree ? " (" + c.degree + ")" : ""} — corde ${tuning.strings[s]}, case ${f}`;

      let dot = null;
      if (c.inSet) {
        const inShape = shape && shape[s] === f;
        dot = svg("circle", { class: "dot" + (shape && !inShape ? " dim" : ""), cx: x, cy: y, r: 11, fill: colorOf(c.degree) }, g);
        const label = state.labels === "degrees" ? (c.isRoot ? "R" : c.degree) : c.name;
        svg("text", { class: "dot-label" + (shape && !inShape ? " dim" : ""), x, y }, g).textContent = label;
        if (inShape) svg("circle", { class: "ring", cx: x, cy: y, r: 14 }, g);
      } else if (state.showAll) {
        svg("text", { class: "ghost", x, y }, g).textContent = c.name;
      }

      g.addEventListener("click", () => {
        Sound.play(c.midi);
        const flash = svg("circle", { class: "ring flash", cx: x, cy: y, r: 13 }, el);
        setTimeout(() => flash.remove(), 400);
      });
    }
  }

  const present = new Set(view.degrees.map(familyOf));
  $("legend").innerHTML = FAMILIES.filter((f) => present.has(f.id))
    .map((f) => `<span><i style="background:var(--${f.id})"></i>${f.label}</span>`).join("");
}

function renderDiagram(d, index) {
  const tuning = catalog.tunings[state.tuning];
  const nStr = d.frets.length;
  const ROWS = 4, SX = 18, SY = 24, LEFT = 30, TOP = 34;
  const W = LEFT + (nStr - 1) * SX + 16, H = TOP + ROWS * SY + 22;

  const box = document.createElement("div");
  box.className = "diagram" + (index === diagram ? " on" : "");
  box.title = "Cliquer pour entendre";
  const el = svg("svg", { width: W, height: H, viewBox: `0 0 ${W} ${H}` }, box);
  const x = (s) => LEFT + s * SX;

  for (let s = 0; s < nStr; s++) {
    svg("text", { class: "strname", x: x(s), y: 10 }, el).textContent = tuning.strings[s];
    const f = d.frets[s];
    if (f <= 0) svg("text", { class: "mark", x: x(s), y: 24 }, el).textContent = f < 0 ? "×" : "○";
    svg("line", { class: "string", x1: x(s), x2: x(s), y1: TOP, y2: TOP + ROWS * SY }, el);
  }
  for (let r = 0; r <= ROWS; r++) {
    const isNut = r === 0 && d.startFret === 1;
    svg("line", { class: isNut ? "nut" : "fret", x1: x(0), x2: x(nStr - 1), y1: TOP + r * SY, y2: TOP + r * SY }, el);
  }
  if (d.startFret > 1) svg("text", { class: "pos", x: LEFT - 8, y: TOP + SY / 2 }, el).textContent = `${d.startFret}fr`;

  for (let s = 0; s < nStr; s++) {
    const f = d.frets[s];
    if (f <= 0) continue;
    const cy = TOP + (f - d.startFret + 0.5) * SY;
    const degree = view.degrees[view.notes.indexOf(d.names[s])] ?? "";
    svg("circle", { cx: x(s), cy, r: 8.5, fill: colorOf(degree) }, el);
    svg("text", { class: "dot-label", x: x(s), y: cy }, el).textContent = d.names[s];
  }

  box.onclick = () => {
    diagram = index;
    Sound.strum(d.midi);
    renderNeck();
    renderDiagrams();
  };
  return box;
}

function renderDiagrams() {
  const section = $("diagramsSection");
  section.hidden = state.mode !== "chord";
  if (section.hidden) return;
  const container = $("diagrams");
  if (!view.diagrams.length) {
    const p = document.createElement("p");
    p.className = "empty";
    p.textContent = view.error ? "—"
      : `Aucune position jouable trouvée (${view.notes.length} notes, écart maximal de 4 cases).`;
    container.replaceChildren(p);
    return;
  }
  container.replaceChildren(...view.diagrams.map(renderDiagram));
}

function render() {
  renderControls();
  renderList();
  renderSummary();
  renderNeck();
  renderDiagrams();
}

function update() {
  diagram = -1;
  view = compute();
  saveState();
  render();
}

// ── Lecture ───────────────────────────────────────────────────────────────

function playCurrent() {
  if (!view.notes.length) return;
  if (state.mode === "chord") {
    const d = view.diagrams[Math.max(diagram, 0)];
    Sound.strum(d ? d.midi : ascendingMidi(view.notes));
  } else {
    Sound.sequence(ascendingMidi(view.notes).concat(ascendingMidi(view.notes.slice(0, 1)).map((m) => m + 12)));
  }
}

// ── Événements ────────────────────────────────────────────────────────────

function bindEvents() {
  document.querySelectorAll("[data-mode]").forEach((b) =>
    b.addEventListener("click", () => { state.mode = b.dataset.mode; filter = ""; $("filter").value = ""; update(); }));
  document.querySelectorAll("[data-labels]").forEach((b) =>
    b.addEventListener("click", () => { state.labels = b.dataset.labels; saveState(); render(); }));
  document.querySelectorAll("[data-acc]").forEach((b) =>
    b.addEventListener("click", () => { state.acc = b.dataset.acc; update(); }));
  $("tuning").addEventListener("change", (e) => { state.tuning = +e.target.value; update(); });
  $("frets").addEventListener("change", (e) => { state.frets = +e.target.value; update(); });
  $("showAll").addEventListener("change", (e) => { state.showAll = e.target.checked; saveState(); renderNeck(); });
  $("filter").addEventListener("input", (e) => { filter = e.target.value; renderList(); });
  $("play").addEventListener("click", playCurrent);

  // Flèches haut/bas dans la liste (ou depuis le filtre) : type précédent/suivant
  const step = (delta) => {
    const items = filteredEntries();
    if (!items.length) return;
    const i = items.findIndex((e) => e.key === state.key[state.mode]);
    const next = items[Math.min(items.length - 1, Math.max(0, (i < 0 ? 0 : i + delta)))];
    state.key[state.mode] = next.key;
    update();
  };

  document.addEventListener("keydown", (e) => {
    const inField = e.target instanceof Element && e.target.matches("input, select");
    if (e.key === "ArrowDown" || e.key === "ArrowUp") {
      if (inField && e.target.id !== "filter") return;
      e.preventDefault();
      step(e.key === "ArrowDown" ? 1 : -1);
    } else if (inField) {
      if (e.key === "Enter" && e.target.id === "filter") playCurrent();
    } else if (e.key === "ArrowRight" || e.key === "ArrowLeft") {
      e.preventDefault();
      state.rootPC = (state.rootPC + (e.key === "ArrowRight" ? 1 : 11)) % 12;
      update();
    } else if (e.key === " ") {
      e.preventDefault();
      playCurrent();
    } else if (e.key === "Escape" && diagram >= 0) {
      diagram = -1;
      renderNeck();
      renderDiagrams();
    }
  });
}

// ── Démarrage ─────────────────────────────────────────────────────────────

createMusiCLI().then((module) => {
  M = module;
  catalog.scale = M.scales();
  catalog.chord = M.chords();
  catalog.tunings = M.tunings();
  $("tuning").replaceChildren(...catalog.tunings.map((t, i) => new Option(t.name, i)));

  loadState();
  if (!catalog.tunings[state.tuning]) state.tuning = 0;
  bindEvents();
  update();
  $("loading").remove();
}).catch((e) => {
  $("loading").textContent = "Erreur de chargement du moteur : " + e.message;
});
