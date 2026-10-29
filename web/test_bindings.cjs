// test_bindings.cjs — vérifie le module WebAssembly sous Node : make test_wasm
const createMusiCLI = require("./musicli.js");

let passed = 0, failed = 0;
function check(ok, what) {
  console.log((ok ? "\x1b[32m✓\x1b[0m " : "\x1b[31m✗\x1b[0m ") + what);
  ok ? passed++ : failed++;
}

createMusiCLI().then((M) => {
  const tunings = M.tunings();
  check(tunings.length === 6 && tunings[0].midi[0] === 40, "Accordages : 6 prédéfinis, E2 = 40");
  check(M.scales().length > 20 && M.chords().length > 20, "Catalogues de gammes et d'accords");

  const am = M.scaleNotes("minor_natural", "A");
  check(am.notes.join(" ") === "A B C D E F G", "La mineur : A B C D E F G");
  check(am.degrees[2] === "3-", "La mineur : 3e degré = 3-");

  const grid = M.fretboard(0, 12, "A", am.notes, am.degrees);
  check(grid.length === 6 && grid[0].length === 13, "Grille 6 × 13");
  check(grid[0][5].isRoot && grid[0][5].midi === 45, "Mi grave case 5 = La (MIDI 45)");

  const bb = M.chordDiagrams(0, "major", "Bb", 3);
  check(bb.length > 0 && !bb[0].names.includes("A#"), "Bb : diagramme orthographié en Bb");
  check(bb[0].midi.length === bb[0].frets.filter((f) => f >= 0).length, "Bb : une hauteur MIDI par corde jouée");

  let threw = false;
  try { M.scaleNotes("inconnue", "C"); } catch (e) { threw = e instanceof Error && e.message.includes("inconnue"); }
  check(threw, "Gamme inconnue : Error JS avec le message C++");

  console.log(`\nResults: ${passed} passed, ${failed} failed`);
  process.exit(failed ? 1 : 0);
});
