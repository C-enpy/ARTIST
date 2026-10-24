# H_Q — Plan QA du lot B1–B6

Auteur : pool-5, profil natif qa-planner. Transmission complète en cinq parties, terminée par `FIN_H_Q`, reçue par le lead le 2026-10-24. Persistance et contrôle : lead. Aucun test écrit/exécuté par l'auteur ; aucune preuve comportementale courante revendiquée.

**Chemin unique de ce lot :** `.opencode/missions/team_mvm73qjo_0000_6wzibc/reports/17-plan-qa.md`. Le chemin historique `.opencode/evidence/mission-gpu-qa/qa-plan.md` cité dans le texte antérieur n'est pas une destination de cette remise et n'est pas écrasé.

Ce plan remplace le livrable absent de `task_mvmbkkq8_0001_y6g986` via la reprise `task_mvmc5s53_000v_ct63sq`, sans effacer l'historique. Le lead doit le commiter et vérifier sa disponibilité avant les auteurs. Ce texte ne vaut ni approbation de politique ni autorisation d'étendre le périmètre.

## Note de provenance — ancien blocage de transport

La version rédigée antérieurement mentionnait « Shell et Code Mode sont fermés : execute est denied », zéro tentative et restitution finale native seulement. Ce bloc est **historique**, pas un refus runtime exécuté : l'auteur confirme zéro tentative à cette date. Il est dépassé par la politique communication v2 et l'accusé `H_Q-TRANSPORT` effectivement reçu, puis cette remise collaborative en cinq parties. Les rapports 14–16 conservent la chronologie. Édition, persistance, commits, tests et délégation restent interdits au planner ; ils ne sont pas requis pour sa remise textuelle.

## 1. Pin observé, sources et décisions acquises

- Source moteur fournie : `989cb995ef6ae96f2eed8f945e2c6ff29bd90d92`.
- Lecture antérieure du planner : `.git/HEAD` → `refs/heads/main` → `354b53f2d4cbdf427aad54069bfc6ef43a5f90f7`.
- Digestion du lead : H_A accepté et commité à `354b53f`, changements documentaires seulement ; source moteur inchangée.
- Digestion ultérieure : protocole B2 commité à `4f8385e`, 17 noms identifiés/remappés statiquement ; aucune campagne H3.
- Ces digestions sont attribuées au lead, pas des vérifications Git ou recherches exécutées par ce profil.
- Avant campagne, le runner qualifie le HEAD intégré réel, son delta et ses empreintes. Ne pas appliquer un ancien pin documentaire comme identité du candidat. Diff gitignore/GrepAI hors périmètre.

Lectures déclarées par l'auteur : AGENTS, principes, registre ADR, sections pertinentes du plan de correction/incidents, fiche du 23 octobre, rapports 03/04/08/09, Reader/extraits de tests, TestUtils et TraceableException. Skill artist-build-validation chargé. Digestions QMD fournies par le lead ; aucune recherche QMD/Serena/GrepAI/Context Mode par ce planner.

Décisions acquises, à ne pas redemander :
- K : B1–B6 confirmés ; enquête shuffle B1 spécifiquement autorisée.
- QA proportionnée avec obligations ARTIST, sans nouvelle infrastructure.
- H_A : calibration Windows puis Reader réel ; close réellement échoué DEFER.
- B2 : protocole/remapping disponibles, pas de campagne H3.
- C2 : OPEN/INCONCLUSIVE, archives absentes dans deux périmètres selon le lead ; aucune clôture.
- ADR-0003/0005 Accepted/partial ; 0008 Windows Debug seulement ; 0004 exclu.

## 2. Obligations et intentions

Obligations explicites :
- **E0** : H_Q commité avant auteurs, H_A disponible avant F8 ; empêcher un travail sans contrat transmis (AGENTS, rapports 03/08/09).
- **E1** : cause shuffle établie ou correction bornée après RED causal ; aucune imputation au moteur sans preuve (fiche B1, rapport 04).
- **E2** : ASan du candidat final et complément GL annoncé ; éviter campagne périmée/sélection incomplète (fiche B2, protocole `4f8385e`).
- **E3** : OS→ifstream→Reader réel, sans nouvelle seam/API ; preuve à la bonne frontière (H_A).
- **E4** : close réel DEFER/non couvert, aucune clôture globale F8 (H_A).
- **E5** : unbind des trois tests nommés avec garde RAII existante (fiche B4).
- **E6** : consolidation après inventaire/allowlist, égalité stricte sans affaiblissement (fiche, rapports 03/04).
- **E7** : C2 reste ouvert sans reproduction ou acceptation humaine d'une fermeture motivée (fiche, rapport 04, digestion récente).
- **E8** : défaut const libstdc++ traité statiquement, MSVC préservé, GCC réel non revendiqué (fiche B6).
- **E9** : RED causal pour correction, GREEN/non-régression, provenance et comptes honnêtes (AGENTS, fiche).
- **E10** : incidents/documentation cohérents, aucune promotion d'ADR ni suppression de l'historique (fiche, principes).

Obligations inférées, distinctes d'une spécification adoptée :
- **I1** : examiner les diagnostics tardifs des mocks, pas seulement le résumé (incidents d'oracle).
- **I2** : séparer cas uniques et répétitions, éviter double comptage (discipline de preuve).
- **I3** : séparer ouverture/extraction/fermeture pour ne pas couvrir la mauvaise branche (H_A, Reader).
- **I4** : requalifier après intégration, éviter preuve antérieure aux mutations (discipline du candidat).

Une ambiguïté de contrat attend une décision humaine.

## 3. Invariants et périmètre

- Correctness/lifetime avant performance, aucune promesse de coût nul.
- Reader : contexte vidé au départ, extraction locale, publication après close vérifié, aucune publication partielle après rejet. EOF normal, vide et erreur réelle restent distincts.
- Types/messages exacts, cardinalités et négatifs non affaiblis.
- Fixture exclusivement possédée ; acquisition/nettoyage vérifiés ; pas de nettoyage voisin.
- Un écrivain par fichier, un build actif par sortie, ASan sous bail exclusif, GL sérialisé sous `artist_opengl` et bail interprocessus du lead.
- Mock/seam ≠ panne OS ; ASan ≠ preuve d'accès GPU ; close réussi ≠ close échoué.
- Zéro test, skip requis, précondition manquante ou preuve périmée ≠ PASS.
- Aucune nouvelle installation/dépendance/infrastructure/service/privilège.

Exclus : ADR-0004, image/performance, Release/nouvelles plateformes, diff gitignore/GrepAI, correction moteur sans défaut causal borné.

## 4. Risques I×P et profondeur

Impact et probabilité sur 1–5, séparés ; churn/absence de tests/branche conditionnelle influencent P, pas I. Aucun score Ensemble modifié.

- **B1** I3/P3=9, moyen : mocks/attentes survivant au test, seeds historiques. Blast radius Loader/ShaderLoader et confiance CPU. Détection : archives tripartites de deux ordres. Mitigation : nettoyage Attribute historique et ordre par défaut documenté. Gap : autres mocks et résultat courant inconnus.
- **B2** I2/P3=6, moyen : binaire obsolète/non instrumenté, environnement permissif ou remapping erroné. Blast radius verdict ASan final. Détection : protocole et remapping transmis. Mitigation : canari, normalisation, clean-first, baux. Gap : pas de run H3, identité binaire/comptages non qualifiés.
- **B3** I2/P3=6, moyen : verrou inefficace, open refusé, erreur assimilée à EOF, traduction/publication incorrecte. Blast radius Reader/preuve F8. Détection : H_A et seam existante. Mitigation : calibration séquentielle/transaction locale. Gap : catch réel non démontré, close DEFER.
- **B4** I2/P2=4, bas : programme lié au cleanup/helper acceptant sous-chaîne. Blast radius trois tests GL/consommateurs allowlistés. Détection : tests nommés/find observé. Mitigation : garde existante/consolidation différée. Gap : allowlist/sensibilité finale non vérifiées.
- **B5** I2/P3=6, moyen : attribution abusive de 20:35/clôture sans cause. Blast radius provenance C2/oracles Reader. Détection : notes et absence dans deux périmètres. Mitigation : baseline exclue du RED causal. Gap : cause/archives primaires non établies.
- **B6** I2/P3=6, moyen : appel const→non const rejeté. Blast radius accesseur libstdc++. Détection : défaut visible dans source lu. Mitigation : branche inactive MSVC. Gap : correctif non qualifié/GCC réel absent.

B1–B4 reprennent la fiche ; B5/B6 sont des estimations proposées. La mention « haut » de la fiche pour 9 ne change pas la grille générale : 9=moyen. Seuil local ≥6 : fiches détaillées néanmoins requises. Aucun item actuel ≥10.

Top3 : B1 faux vert dépendant de l'ordre ; B3 sonde non discriminante ; B2 campagne périmée/faussement verte. Réévaluer à H1/H2/H3, aux surprises et changements de candidat. Si ≥10, actualiser fiche/profondeur/budget ; ne pas préserver du moyen au détriment du haut ou du buffer.

## 5. Scénarios et oracles proposés, non exécutés

### E0/E9/I4 — identité et gates
- Nominal : contrats commis reçus, candidat/delta/source/binaire concordants, baux disponibles ; oracle provenance vérifiable.
- Négatif : contrat absent/hash divergent/zéro découverte/skip requis/bail absent ; INCONCLUSIVE, pas PASS recyclé.

### B1 — shuffle/lifecycle
- Nominal : suite CPU complète ordre par défaut puis seeds demandées `20261017` et `7`, seed effective et ordre conservés ; cas attendus, zéro échec/skip requis/diagnostic tardif.
- Négatif : reproduction pré-correctif, isolation prédécesseur/état contaminant ; attente précise en échec puis disparition après correctif borné, négatif conservé.
- Heuristiques : contaminant→victime, répétition, nettoyage sans court-circuit après sortie anticipée si pertinent.
- Un filtre isolé vert ne ferme pas un défaut d'ordre ; deux seeds ne prouvent pas toutes les permutations. Après deux essais sans information nouvelle : nouvelle expérience/retour lead.

### B2 — ASan et 17 GL
- Nominal : protocole `4f8385e` sur H3, reconstruction propre, découverte fraîche, présence effective des 17 noms/remappings.
- Négatif : sensibilité canari et héritage hostile selon harnais adopté ; comportement instrumenté/diagnostic/code non nul attendus, canari invalide interdit PASS ASan.
- Recouper sélection annoncée/découverte/exécution/JUnit/logs/skips/uniques/répétitions. Remapping statique ≠ découverte/exécution. Les 17 noms ne sont pas réinventés par ce plan : reprendre le protocole intégré. ASan ne prouve pas les accès GPU.

### B3 — H_A accepté
Préconditions : H_Q/H_A commis, fixture possédée, paramètres/commandes avant essai, fichier non vide et toolchain identique calibration/Reader.
1. Sans verrou : open, octets exacts, close réussi.
2. Second handle/partage autorisant open, verrou exclusif de plage ; calibrer refus ReadFile recouvrant la plage, opération/code conservés. ERROR_LOCK_VIOLATION reste hypothèse.
3. ifstream : open réussi, badbit inchangé, exception/gcount/rdstate/bad/eof/fail ; vérifier rétablissement après déverrouillage.
4. Ensuite seulement Reader réel sans extraction scriptée : contexte prérempli vidé, type/message exacts, aucune publication partielle.
5. Si downstream exercé : code initial vide et zéro compilation GL après rejet par moyens existants.

- Nominal : octets exacts sans verrou/après déverrouillage, close réussi avant publication.
- Négatif : opération réelle ouverte puis défaillante, causalement reliée au verrou, contexte vide après rejet.
- ifstream::failure authentique puis traduction : preuve ciblée catch.
- Erreur OS mais EOF/publication vide ou tronquée : candidat défaut causal, pas preuve catch ; retour lead avant correction.
- Open refusé/sonde non discriminante : INCONCLUSIVE catch réel.
- Interdits : changer masque, setstate, throw direct callback, rdbuf remplacé, handle caché fermé sauvagement, nouvelle seam/API.
- RAII des seuls handle/verrou de test ; déverrouiller avant cleanup. Non-régression : vide, dernière lecture courte, multiple exact4096, oracle byte-for-byte.
- Message complet établi par contrat/exception calibrée, jamais sous-chaîne. Pas d'OOM global : seam bad_alloc=non-traduction injectée, pas pénurie réelle.

### E4 — close réellement échoué
- Nominal : close réussi avant publication.
- Négatif futur : lecture intégrale puis close authentiquement échoué, aucune publication.
- Oracle futur branche fail() : `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\nClosing the file failed.` ; close jetant ifstream::failure = branche de traduction distincte.
- Verrou lecture, absence, suppression/rename, double-close ou close depuis callback ne prouvent pas ce négatif.
- **GAP-defer/DEFER non couvert**, propriétaire lead+build/OS, dispositif/décision séparés. Date à fixer lors de reprise, avant toute revendication de clôture globale F8. Impact F8 partiel ; aucune infrastructure nouvelle ici pour effacer la dette.

### B4-U — unbind
- Nominal : UsePassTest/UseNextPassTest/ResetPipelineTest avec garde existante, débind avant destruction et contexte vivant.
- Négatif : sortie anticipée du scope par mécanisme existant ; cleanup/état GL attendu, erreur initiale non masquée. Preuve GL réelle ciblée ; lecture seule insuffisante.

### B4-E — helpers
- Nominal : type/message complet exact acceptés.
- Négatifs : préfixe/suffixe supplémentaire, sous-chaîne seule, mauvais type, absence d'exception rejetés.
- Inventaire/allowlist avant consolidation ; TestUtils immuable première vague. Égalité stricte partout dans les consommateurs retenus. ViewTests non localisé ≠ permission sur un sous-arbre.

### B5 — C2
- Nominal futur si archives transmises : identité/conditions 20:35 et20:38 rattachées aux octets.
- Négatif : provenance absente/setup fichier invalide = précondition manquante, pas RED/imputation moteur.
- Pas de nouvelle chasse aux archives déjà absentes dans deux périmètres. Un état-fichier causal étudié dans le périmètre approuvé n'établit pas rétroactivement le déclencheur historique.
- OPEN/INCONCLUSIVE ; fermeture motivée seulement sur acceptation ultérieure, sinon DEFER avec propriétaire/échéance fixés par lead. Vert courant/rebuild ne prouvent pas la cause.

### B6 — libstdc++
- Nominal : appel const sous MSVC, std::string possédé, comportement préservé.
- Négatif : motif const/non-const pré-correctif établi statiquement ; compilation négative pertinente si disponible, rejet pour le motif attendu et non toolchain cassée.
- Oracle : chaîne const-correcte, retour par valeur préservé, condition #if recoupée. Pas de macros libstdc++ forcées sous MSVC/installation implicite.
- Sans GCC réel : argument statique seulement, aucun PASS GCC. Une tranche corrective doit préciser la preuve RED réalisable avant son implémentation ; impossibilité = blocage explicite, pas RED inventé.

### E10 — documentation
- Nominal : statuts/candidat/portée/comptes/limites concordants.
- Négatif : ancien candidat/close couvert/campagne non exécutée dite verte = incohérence à corriger avant sortie.
- Reindex/contrôle par lead ; contrôle texte ≠ enforcement natif.

## 6. Intégration, niveaux et exclusivité

1. H_Q/H_A commis et transmis.
2. Première vague ownership disjoint confirmé, TestUtils immuable.
3. H1 intégration réelle première vague.
4. Inventaire/allowlist puis helper strict.
5. H2 candidat intégré gelé ; correctness/gaps indépendants peuvent analyser en parallèle.
6. H3 propositions acceptées réellement intégrées.
7. Campagne finale, audit fresh puis clôture selon obligations ARTIST.

CPU B1/B3/B5 ; compilation/statique B6 ; OpenGL réel ciblé B4-U/B2. Fixtures/mocks existants, pas d'image/performance/nouveau niveau de mocks adopté. Generator après intégration, jamais substitut au RED auteur ; auto-relecture ≠ revue indépendante. Un seul runner/build par sortie, bail GL interprocessus.

## 7. Registre bidirectionnel

Obligation → localisation / raison / état :
- E0/I4 : rapports03/04/08/09, HEAD/delta final ; contrat/identité conditionnent application ; H_A commité selon lead, H_Q à persister lors de la remise.
- E1/I1 : suites Loader/ShaderLoader, chemins exacts à confirmer par auteur ; ordre/état contaminant ; GAP historique seulement.
- E2/I2 : protocole4f8385e, harnais build/ASan ; instrumentation/sélection/environnement ; static-presence attribuée lead, campagne H3 GAP.
- E3/I3 : `ARTIST/include/graphic/opengl/pipeline/component/shader/Reader.hpp`, OpenGLShaderReader<Classic>::on, detail::readShaderFile/extractShaderSource ; extraction→traduction→publication ; static-presence, réel GAP.
- E3 fixture : ReaderTests.cpp tryAcquireOwnedRoot/tryCleanupOwnedRoot/tryGuaranteeAbsentPath ; acquisition/cleanup bornés ; static-presence, pas executed.
- E4 : readShaderFile après close ; close distinct extraction ; GAP-defer.
- E5 : `ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp`, trois tests nommés ; liaison/cleanup ; unassessed final.
- E6 : `ARTIST/tests-common/include/TestUtils.hpp:expectSpecificError`, allowlist à établir ; find accepte sous-chaîne ; static-presence d'écart, aucun FAIL exécuté.
- E7 : ReaderTests/archives incidents ; provenance/setup/comportement distincts ; OPEN/INCONCLUSIVE, archives absentes selon lead.
- E8 : `ARTIST/include/common/exception/TraceableException.hpp:getStackTraceString/format_stacktrace` ; const→nonconst visible ; static-presence, GCC réel GAP.
- E9/E10 : manifestes/incidents/docs du lot ; évite clôture/promotion sans preuve ; GAP final.

Test/preuve → obligation / limite :
- Archives shuffle PRE_G1/RED/GREEN deux seeds → E1/I1 ; historique, pas final.
- ScriptedStreambuf/scriptedExtraction → sous-contrat E3 ; autre istream, pas filebuf/OS.
- PublicationProbeContext lu → publication E3 ; présence, pas close échoué.
- expectSpecificError → E6/consommateurs ; helper/égalité faible, pas exécution.
- Baseline20:35 → E7 ; pas RED causal F8.
- red-exact20:38 → F8 historique ; pas H_A.
- S-GSS historiques → E8 retour par valeur ; pas libstdc++.
- Protocole/remapping17GL4f8385e → E2 ; présence attribuée lead, pas H3.
- Campagnes normales/ASan historiques → contexte E2 ; pas final.

Orphelins **unassessed**, pas « aucun » : inventaire/diff tests à recouper au candidat. Doublons faibles : répétitions comptées comme nouveaux contrats, seam=OS, JUnit/log/résumé comptés comme exécutions distinctes. Autre candidat ou mutations postérieures = preuves périmées, sauf portée historique explicite. Aucun comportement déclaré COVERED ici.

## 8. Qualification des preuves

Chaque preuve retenue : HEAD complet/delta/SHA256 sources-binaire-config-artefacts ; source/date/commande/runner/environnement ; attendu/observé ; découverts/exécutés/échoués/skipped et uniques/répétitions ; critère/raison discriminante/limites/verdict.

- Lectures antérieures planner24octobre : read/glob/skill, HEAD lu354b53f, SHA/delta non calculés, zéro test, comptes non mesurés.
- Rapports03/04/08/09 : autorité documentaire, aucun essai runtime dans ces rapports.
- Moteur989cb995/H_A354b53f : attribution lead, pas diff indépendant du planner.
- B2 protocole4f8385e/17remappings : digestion lead, static-presence, pas H3 ; métadonnées au protocole.
- C2 archives absentes deux périmètres : constat lead, pas reproduction/clôture ; périmètres détaillés dans rapport11.
- Campagnes historiques : artefacts non intégralement relus par planner, insuffisants pour COVERED courant.

PASS vérifie un critère ; FAIL reproduit l'écart ; INCONCLUSIVE précise le manque. Zéro test/skip requis jamais PASS. L'accusé de transport ultérieur ne transforme aucune de ces preuves en preuve moteur.

## 9. Runner séparé

Lead distinct de confiance, explicitement attribué. Avant run : inspecter CMake/presets/Conan/CI ; Conan2 avant CMake, compilateur/STL/ABI/CRT/architecture/SDK/dépendances, sonde format+stacktrace compile/link. Pas de migration implicite.

Commandes documentées proposées, non exécutées par le planner :
```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -TestLabel unit -Jobs 1
ctest --test-dir out/build/windows-msvc-debug -N
ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error
ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error --parallel 1
```
ASan : commandes exactes protocole4f8385e, confirmer sélection contre découverte H3 ; pas d'option inventée. Aucune de ces chaînes n'est une autorisation d'installation implicite ni un script à exécuter automatiquement.

## 10. Budget proposé, non ratifié

Hypothèse24h-personnes : setup6h (identité/contrats/environnement/fixtures/baux/calibration) ; B1 2,5h ; B2 3,25h ; B3lecture hors close2,75h ; B4 1,5h ; B5sans nouvelle chasse1h ; B6 1,25h ; intégration/registre/relecture1,25h ; buffer investigation/panne/ré-audit4,5h.

Après setup18h : planifié13,5h=75%, buffer4,5h=25%. Setup compté une fois ; calibration pas recomptée B3. D'abord sensibilité/provenance peu coûteuses, B1/calibration F8 ; campagnes coûteuses après. Nouvelle infrastructure ou capacité insuffisante : budget irréalisable en l'état, arbitrage explicite, pas d'effacement du buffer. Hypothèse de planification, pas dépense ni durée acceptée par silence.

## 11. Gates, décisions et sortie

Avant auteurs/exécution :
1. Persister/commiter H_Q et vérifier réception effective.
2. H_A commité, identité/delta sources→binaire qualifiés pour l'essai.
3. Protocole B2/17GL joint aux entrées runner, pas redemandé.
4. Ownership/baux/fixture exclusive confirmés.
5. Après H1, allowlist exacte avant consolidation.
6. Approbation/reprise native applicable vérifiée ; ce plan ne les lève pas.

Décisions ouvertes : capacité/budget ; allowlist helpers ; éventuelle fermeture motivée C2 (sinon OPEN/INCONCLUSIVE) ; date/dispositif séparé close ; correction nouvelle révélée causalement par H_A avant implémentation. K/H_A/QA proportionnée ne sont pas à redemander.

Sortie :
- Chaque branche traité/fermeture motivée acceptée/GAP-defer avec propriétaire-échéance-impact.
- RED causal pour correction, GREEN/non-régression final ; suites prévues non nulles, zéro FAIL/skip requis, logs/JUnit recoupés.
- B1 limité aux ordres exécutés ; B2 instrumentation/sélection exécutées H3, protocole insuffisant.
- B3 calibration discriminante OS→ifstream→Reader, sinon INCONCLUSIVE/défaut causal transmis ; close réel DEFER/non couvert, aucune clôture globale F8.
- B4 unbind GL réel/égalité stricte allowlistée ; B5 histoire non réinventée ; B6 MSVC/statique distincts, pas PASS GCC.
- H1/H2/H3 matérialisés, audit fresh applicable ; incidents/docs cohérents, pas promotion ADR, reindex attribué lead.

## Contrôle de remise par le lead

Les cinq parties et FIN_H_Q sont reçus via le canal testé. Normalisations de persistance : destination courante explicitée, ancien blocage de transport annoté historique, tableaux reformulés en listes sans changement d'oracle ; restrictions du runner et B6 clarifiées sans autoriser de preuve manquante à devenir RED. Le budget est arithmétiquement cohérent, reste une hypothèse. Aucun budget nouveau ni extension de tests/infrastructure adopté par ce document.

Cette remise est un **plan QA préalable**, pas un PASS du candidat. Aucun fichier/test/commit produit par le planner ; persistance/commit et toute exécution appartiennent au lead. Les auteurs ne démarrent qu'après vérification du commit et de leurs entrées effectives.
