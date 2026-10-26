# 85 — Contrat stable et inventaire adoptés ; barrière QA avant auteurs

2026-10-26. Suite aux décisions opérateur [83](83-recentrage-operateur-fichiers-stables.md) et aux entrées [84](84-entree-ro-inventaire-et-contrat-stable.md).

## État et remises intégrées

HEAD observé : `febe2ee1aaf9dc963ecf7488fbb7d79ed08d69e1`. Aucun changement source de cette tranche. Diffs étrangers inchangés : `.gitignore`, `.grepai`, AGENTS/profils/harness/validate/tests agentic ; rename GrepAI seul staged, à ne pas annexer.

Remises RO pool-13/engine-architect et pool-7/project-manager récupérées dans leurs contextes natifs, restitution finale comprise. Fragment PM **8/8 retrouvé** : aucune nouvelle enquête ni retransmission nécessaire. Sessions sources : `ses_e8682b9a8ffe10kcn1gS02Ic3Q` et `ses_e87558e7affeOtee8XOFUkxYPZ`. Aucun build/test/édition par ces préparateurs. La lecture lead de Reader.hpp et des tests596/657/693 recoupe la migration proposée ; leurs témoignages ne valent pas validation exécutée.

## Contrat adopté pour la tranche

Le lead adopte la proposition portable de l'architecte, sans changement de comportement recherché :

1. Fichier stable pendant lecture, contexte non modifié concurremment. Aucun verrou, versioning, handle natif ou synchronisation nouveau.
2. Conserver mode texte, chemins et diagnostics actuels. Lire les caractères délivrés par le flux, pas promettre l'identité des octets disque après traduction CRLF. Vide valide ; court,4096,8192,12325 et queue partielle exacts. NUL préservé dans la string ; compilation GL au-delà NUL hors tranche (`glShaderSource` sans longueur).
3. Vider le contenu précédent avant ouverture, accumuler dans une string locale. Garder `read/gcount`, masque `badbit`, rejet fail sans EOF et traduction ciblée `ifstream::failure`. EOF normal fail+eof sans badbit est accepté. Ne pas élargir le catch ni remapper `bad_alloc`.
4. Après lecture validée, effacer le seul résidu EOF, fermer explicitement, vérifier l'état, puis publier. En échec : pas de source stale/partielle publiée ; ownership local et destruction de fichier conservés. Garantie du setter standard ; extension arbitraire lançant après mutation hors garantie.
5. `readShaderFile(context)` appelle directement `extractShaderSource`. Retirer `SourceExtractor` et le paramètre callback, sans remplacement par framework/seam/template. API publique `OpenGLShaderReader<Classic>::on` inchangée. Les symboles `detail` ne constituent pas ici une interface externe garantie ; le header est installé, donc consommateurs externes **non exclus** et compatibilité de leur usage interne non revendiquée.

Toute correction comportementale supplémentaire exige un RED causal avant implémentation ; sinon STOP au lead. La tranche prévue est un refactoring/retrait avec caractérisations **avant** restructuration puis non-régression, pas un correctif inventé ni une exemption héritée des probes.

## Inventaire et disposition explicite des oracles

**KEEP** : Reader public, algorithme/contrôles/catch/ordre fermeture-publication ; fixtures isolées ReaderTests27–151/273–355 et régressions486/510/536/555 (acquisition atomique, setup fail-stop, absence garantie, cleanup borné/voisins). MockReader/ShaderTests, consommateurs Shader/Loader/ClosedExecution/lifetime et component-tests restent présents. Aucun contexte/Loader/Shader/Pass refondu.

**MODIFY** : Reader.hpp et ReaderTests.cpp, propriétaire unique. Caractériser sur l'ancien code le contenu court/8192/texte/NUL, remplacement de source préremplie et absence de stale au rejet ; conserver vide/court/bloc/multibloc et diagnostics. Les nouveaux tests verts avant refactor sont des caractérisations, jamais un RED.

**REMOVE actif explicitement adopté** : les deux tests `ExtractionFailure_SentinelTranslatedWithExactMessage` et `ExtractionFailure_BadAllocStaysUntranslated`, `ScriptedStreambuf`, sa globale et `scriptedExtraction`. Ils injectent un autre istream et caractérisent la seam retirée, pas une erreur authentique du fichier. Leur retrait perd la caractérisation exécutée de traduction/bad_alloc à cette frontière : preuves historiques conservées, contrôles production gardés et revue statique ciblée requise. Pas de nouveau mécanisme artificiel pour les maintenir.

**REMOVE/MIGRATE adopté** : `PublicationProbeContext` et `CloseHappensBeforePublication_ObservableThroughVirtualSetter`. Le setter supprimant le fichier observe le partage Windows, pas une fermeture portable ni son échec. Remplacer l'intention contenu par un test portable via le contexte standard, sans tentative de suppression pendant publication. L'ordre fermeture-vérification-publication reste exigé dans le code et en revue ; son ancienne preuve instrumentée ne se transforme pas en preuve portable. L'échec authentique d'extraction, le catch historique et l'échec authentique de close avec non-publication restent **gaps nommés non résolus**, pas PASS ni fermeture administrative de B3.

**REMOVE nominatif, après contrôle final des consommateurs** :

- `scripts/probes/qa-f8-stdio/probe.cpp`
- `scripts/probes/qa-f8-stdio/probe_contracts.hpp`
- `scripts/probes/qa-f8-stdio/native_read.hpp`
- `scripts/probes/qa-f8-stdio/controls.cpp`
- `scripts/probes/qa-f8-stdio/PROBE-PROTOCOL.md`
- `scripts/probes/qa-f8-stdio/RED-PROTOCOL.md`
- `scripts/probes/qa-f8-close/close_probe.cpp`
- `scripts/probes/qa-f8-close/wait_policy.hpp`
- `scripts/probes/qa-f8-close/wait_policy_baseline.cpp`
- `scripts/probes/qa-f8-close/wait_policy_contract.cpp`
- `scripts/probes/qa-f8-close/controls.ps1`
- `scripts/probes/qa-f8-close/PC1-PROTOCOL.md`
- `scripts/probes/qa-f8-close/PARENT-ORACLES.md`

PM : graphe interne stdio probe→native_read/probe_contracts/extractor, controls→native_read ; close probe/deuxTU→wait_policy, controls→CLI. Pas de consommateur production/CMake/CI exact trouvé ; CMake ne globbe que les tests dans leurs sous-arbres. Recontrôler avant suppression ; un nouveau consommateur bloque son retrait, pas un prétexte à étendre le nettoyage.

**HISTORICAL préservé** : out/**, docs/_meta/**, rapports/research/anciennes missions/commits, manifestes annulés et logs/binaries. **KEEP partagé** : out/tooling/run-build.py et msvc-env.cmd. **FOREIGN intouché** : diffs listés ci-dessus. Aucun purge/reset/revert global, suppression de branche/worktree/archive ou nouveau service/backend. Les protocoles locaux retirés restent récupérables aux commits historiques, notamment `56296fe`/`d03f996` ; builds seuls rapport81, aucun runtime.

## Graphe PM validé et ownership

Méthode PM/LLM ; scores distincts, non probabilités. Scores bruts→retenus C / R validés lead :

- Préparation PM64→64/R72, architecte66→66/R74 : déjà remises RO, pas nouvelle admission doublon. La fiche PM architecte78 était une estimation proposée ultérieure, pas une modification rétroactive de l'admission66.
- Q_STABLE qa-planner RO61→61/R72 : amendement préalable, permissions/transport effectifs à contrôler. Lead persiste et commit.
- G_STABLE lead52→52/R70 : adoption+QA committées avant worktrees auteurs.
- D_READER cpp-engineer84→84/R80 : **Reader.hpp+ReaderTests.cpp seulement**, fresh isolé ; caractérisations avant refactor, runner lead seul.
- C_PROBES cpp-engineer54→70/R56 : **les13 fichiers seulement**, fresh isolé, contrôle consommateurs puis retrait. Pas de RED comportemental fictif pour suppression.
- I_CANDIDAT lead50→50/R67 : intégration effective, contrôle diff et identité avant consommateurs.
- V_STABLE runner lead68→68/R78 : pas de développement ; sorties nouvelles, build unique par sortie, GL/ASan sériels sous bail.
- R_STABLE test-reviewer fresh66→66/R76 : audit final natif sur contrat/candidat/preuves, hors justifications auteurs ; admission restrictive vérifiée.
- DOC_STABLE lead45→45/R55 : photographie active après preuves ; pas de nouveau worker nécessaire.

Graphe : remises PM||architecte → Q/adoption → G committé → **D||C** → intégration I → validation V → revue/audit → documentation finale. D et C possèdent des fichiers disjoints. Les runs caractérisation/RED éventuel/GREEN appartiennent au même cycle D et au runner lead ; pas de builds concurrents. Aucun consommateur publié avant intégration de ses entrées.

Exigence projet QA maintenue : amendement préalable ; après intégration, examen des gaps par test-engineer restrictif, propositions seulement sans toucher production ; score/ownership d'un éventuel code de tests à fixer avant admission, plancher70. Audit final fresh et ré-audit si correction. Pas de pipeline expérimental ni réouverture OS ; conflit de politique effectif sera exposé, pas silencieusement levé.

## Validation prévue, non exécutée à ce jalon

Runner lead : environnement/presets/protocole existants, caractérisations avant refactor puis build unit+component, découverte réelle et sélection figée pré-run, `--parallel 1 --no-tests=error`, logs/JUnit complets. Les placeholders du PM ne sont pas des argv autorisées : figer chemins/commandes/candidat avant chaque série. ASan proportionné au nouveau diff, aucun ancien555/17 automatiquement hérité ; si campagne complète nécessaire, découverte=sélection=exécution,17 historiques chacun une fois, zéro failure/error/skip et diagnostics tardifs contrôlés. Aucun rerun cosmétique.

Limites : aucune preuve GCC/Linux/Release/image/performance nouvelle. ADR0001/2/4/6/7 Proposed,0003/5 Accepted partial,0008 Windows Debug seulement. B1 revue finale encore requise ; B5/C2 disposition acceptée34 conservée. Pas de PASS global. Prochaine étape autorisée : QA RO ciblée puis commit de son amendement **avant tout auteur** ; code et suppression encore NOT_RUN.
