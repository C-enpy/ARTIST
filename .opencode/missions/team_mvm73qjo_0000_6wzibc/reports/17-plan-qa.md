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

## Amendement du25octobre — reprise initiale B1/B3/B5

Proposition Q par pool-6/qa-planner, cinq fragments natifs complets reçus ; persistance et décisions par lead. Analyse locale seulement, aucun test/édition/exécution par planner. Admission et transport après restriction vérifiés au rapport32 de team_mvnxps48_0000_dfde7a ; pas garantie sandbox. Cet amendement actualise les dispositions historiques ci-dessus, sans refaire le plan ni rouvrir B2/B4/B6. Source82bf52d, HEAD documentairefa7a949 au brief ; pins historiques ne valent pas identité future.

### Exigences, décisions et invariants

KEEP E0/E2/E5/E6/E8/E9/E10 et I1–4 au candidat acquis. MODIFY E1 : deux mécanismes causaux réduits, pas toutes permutations. MODIFY E3 : calibration distincte du futur Reader ; E4 reste obligation authentique, aucun DEFER reconduit tacitement. MODIFY E7 : provenance positive et disposition explicite.

Explicites : contrat/QA commis avant auteur ; RED causal avant correctif ; erreur/EOF/publication distingués ; close réellement échoué à traiter ; disposition C2 explicite. Invariants/intent : aucun préfixe publié après rejet, owner unique/close une fois, fixtures possédées et voisins préservés. Inférences et non nouvelles specs : recovery vérifie contamination ; M3 explore préfixe acquis ; comparaison modes détecte régression texte ; cleanup préserve erreur primaire. Setter arbitraire jetant/mutation concurrente hors garantie proposée.

Décisions opérateur du rapport34 (même répertoire que32) : **calibration stdio et recherche OS close bornées autorisées**, après ce plan commité et contrat de sonde ; **B5/C2 fermeture administrative motivée acceptée**, pas causalité exhaustive. Contrat Reader, messages, retrait catch/seam NON adoptés. Aucun correctif Reader autorisé par ces décisions.

### Risques QA I×P (indépendants des scores PM)

- Erreur assimilée EOF/publication : I4×P4=16. Déclencheur open réussi puis erreur native ; blast radius chargement shader ; détection sonde25 OS33→EOF ; mitigation contrôle actuel insuffisant, calibration M0/M1/M2 ; gap stdio non calibré, aucun verdict correction.
- Close/lifetime/publication : I4×P3=12. Déclencheur close échoué ou cleanup après erreur ; blast radius flux/contenu ; détection statique et scripted insuffisante ; mitigation owner unique/publication après close, recherche OS bornée ; gap M5 authentique absent, aucun double-close artificiel.
- Fausse attribution C2 : I3×P3=9. Détection provenance28, garde exists=false avant Reader, populations13≠11, exit pipeline masqué ; préserver manques, pas replay. Disposition motivée acceptée au34.
- B1 résiduel I3×P2=6 : revue mécanismes/populations/logs sans sweep. Identité/preuves I3×P3=9 : manifestes avant réemploi, pas programme transverse. Réévaluer après calibration/surprise, sans sacrifier un risque haut ni consommer le buffer d'avance.

### Scénarios et oracles

B1 : H négatif A4RUN/2uniques, B6RUN/3uniques, exit1, défauts compileFalse puis8/5 au second tour ; N mêmes sélections exit0. Rapports29/30 satisfont ces témoins, pas rerun demandé. XML dernière itération seulement ; stderr A deux diagnostics négatifs attendus, avertissement GMock non-intéressant pas échec automatique. Audit final attendu.

Calibration autorisée : M0 fichiers possédés0/court/4096/8192/12325/4608, CRLF/Ctrl-Z/BOM/NUL, chemins espaces/nonASCII. Comparaison au comportement texte actuel caractérisé, pas choix binaire silencieux. M1 : ouverture réussie puis LockFileEx recouvrant lecture ; témoin ReadFile erreur33, fread n/feof/ferror/errno avant cleanup. Succès expérimental = distinguer erreur native de EOF. ferror perdu = option rejetée ; open refusé/erreur native absente = expérience INCONCLUSIVE selon cause. Aucun RED fictif du probe. M2 unlock/recovery : mêmes octets exacts, ressources libérées, voisins intacts. Zéro sans EOF/erreur : règle proposée, pas panne OS prouvée. M3 seulement après M1 et hypothèse justifiant préfixe réellement acquis ; sinon GAP, pas sweep préfetch.

Futur Reader conditionnel : contrat adopté/messages complets avant RED ; nominal lecture+close puis publication unique exacte, vide légitime. Négatif absence et M1 authentique, contexte prérempli vidé sans préfixe, type/message exacts adoptés. RED Reader actuel puis GREEN même verrou/recovery, oracle inchangé. Si downstream exercé, absence de compilation après rejet par moyens existants ; mock ne prouve pas GPU. M4 catch ifstream authentique toujours non prouvé : migration nécessitera substitution explicitement acceptée, jamais «catch exercé». M5 : lecture achevée puis close authentiquement échoué sur owner valide ; aucune publication, close une fois, primaire préservée. Dispositif non acquis : recherche bornée puis décision résiduelle, pas fermeture tacite. Flags/throw/rdbuf/masque/handle fermé artificiellement REJECT comme preuves OS/catch/close ; bad_alloc non traduit = sous-contrat, pas pénurie réelle.

C2 : provenance28 établit défaut fixture, forte explication Empty/probable MultiBlock ; fermeture motivée34 acceptée, primaires manquants archivés. Réouverture sur primaire contradictoire ou symptôme nouveau identifié, pas verdict moteur ni reconstruction actuelle du passé.

### Registre bidirectionnel et orphelins

E1/I1/I2 → shader/LoaderTests.cpp(callCreate46,setContextShaderId106,Failure121), pass/LoaderTests.cpp(validContext), graphic/pipeline/ShaderLifetimeTests.cpp(PassDestructorReleasesProgramThenShaders) sous ARTIST/tests/src/graphic/opengl/pipeline/component pour les deux Loader ; raisons writer/default puis observer, preuves A-H/A-N compilation et B-H/B-N handles. COVERED exécuté attribué lead seulement pour deux mécanismes. H SHA3d4fb31ee463a1c44370e9ba051e584784bf2f5becbf584341169d71d2ea1388 ; N SHA1b0fc02956d424f7fa6de02a91211b8e14f4295784d3563342be7c064e3950b5. Commandes/manifeste29/30 et out/qa-b1/causal-20261025-01, runner lead25octobre18:25:49Z, CPU mocks.

E3/I3 → Reader.hpp extractShaderSource/readShaderFile/on : présence statique ifstream/fail&&!eof/clear/close/setter/catch ; sonde25 défaut causal attribué lead, pas correction. M0–M3 GAP à exécuter selon contrat. E4/M4/M5 → close/catch : présence statique, négatifs authentiques GAP. E7 → historique ReaderTests.cpp helper/Empty/MultiBlock et provenance28 : documentaire positive, disposition34, causalité complète non acquise.

Inverse : A/B supports stdout/stderr/XML/résumé = mêmes runs, pas preuves indépendantes supplémentaires. ScriptedStreambuf/SourceExtractor/PublicationProbeContext → sous-contrats, jamais OS/catch/close. Migration/suppression à décider, pas automatique. C1083 historique → environnement, aucun RED produit. ASan555/anciens verts → leurs candidats seulement. Orphelins futurs **unassessed** tant que diff tests absent. Conserver pour toute preuve candidat/delta/SHA source-binaire-config-logs, source/date/commande/runner/env, attendu/observé et comptes uniques/failed/error/skipped.

### Budget, barrières et sortie

Capacité100 proposée, ni coût ni durée approuvés : setup25 ; reste75 dont planifié56,25 (75%) et buffer18,75 (25%). Planifié B1/B5=8, calibration20, recherche close10, futur Reader conditionnel12, registres/audit6,25. Buffer investigation7/panne5/ré-audit6,75. Pas d'engagement M3 coûteux. Dépassement recherche close/alternative10 ou campagne indispensable hors solde → réestimation opérateur, pas réduction risque haut/buffer artificiel.

Barrière1 : ce plan commité et contrat de sonde, développement C≥70, commandes exactes dérivées base compilation/protocole25 et approuvées avant runner lead. Recherche close indépendante RO possible, aucun test implicite. Barrière2 après résultats : adoption distincte Reader/messages/catch/seam et disposition M5, amendement commité avant worktree auteur ; TDD/non-régression. Producteurs intégrés avant consommateurs. Un build par sortie ; fixtures possédées, GPU/ASan sous bail si nécessaire.

Nouveau diff intégré : sélection pré-run, Windows Debug CTest --parallel 1 --no-tests=error et JUnit/logs complets selon risque/protocole existant, pas campagne acquise rejouée sans pertinence. Si ASan complète nécessaire : découverte=selection=execution,17historiques chacun une fois, zéro failure/error/skip, canari pertinent et diagnostics tardifs. Sinon FAIL écart ou INCONCLUSIVE précondition, pas transfert ASan555.

Sortie : B1 preuves bornées recoupées ; B3 preuve ou gap explicitement soumis pour chaque obligation ; C2 disposition acceptée archivée. Audit test-reviewer natif fresh dédié, contrat/candidat/preuves sans justifications auteur, profil/transport vérifiés, aucune exécution autonome. Proposition Q intégrée ne vaut ni Reader prêt auteur ni PASS moteur. ADR0001/2/4/6/7 Proposed,0003/5 partial,0008 Windows Debug seulement.

## Amendement close OS en enfant dédié — décision52,26octobre

Proposition Q_CLOSE par pool-6/qa-planner : quatre fragments/FIN et complément focalisé reçus ; persistance lead, aucune édition/test/API/MCPexterne/claim/délégation par planner. Contexte conservé de préparation **non indépendant**, admissionrestrictive/lecture53/transport effectifs vérifiés54. HEAD e1508b9/source82bf52d attribués brief, pas Git/hashrecalculés par QA ; référencesMicrosoft/CRT digestlead, pas nouvelle recherche externe. Cet ajout actualiseE4 sans refaire17 ni rouvrirB1/B2/B4/B5/B6/C2.

### Exigences et limites

MODIFY E4-calibration : décision52 adopte seulement refus délibérément configuré HANDLE_FLAG_PROTECT_FROM_CLOSE sur emprunt valide de FILE*enfantdédié ; exception ciblée à l'interditflags, tous autresEOF/ferror/throw/handleinvalide restent exclus. **KEEP E4-ReaderM5/nonpublication et M4catch NON prouvés** ; calibration≠M5couvert. OwnerFILE/fd/enfant unique, une fclose, pas retry/_close/CloseHandleemprunté/réutilisationvaleurhorsenfant. Parent seul owner de ses handles supervision, signalfinavantstatut/cleanup, timeout≠terminaison, aucunTerminateProcessfallback. CRTchargée etretour/exception à observer sanscodeprédéterminé. Nominal/cible distincts, fixtures/voisins possédés ; contrat/protocole/QAcommisavantfreshworktreeauteur, commandes approuvées avant runnerlead.

Inférences proposées : corrélationPID/bras/ressource/étapes pour ordre, logs découplés d'attente/pipesdrainés ; sortie main ou exit0 ne prouve ni finDLLdetach ni fclose0. Intent = pas secondefermetureemprunt ni suppressionfixtureenfantvivant. Compile/link fixe structure/CRT ; refus/vieprocessus runtime, aucun assettype/GPU/Reader changé. Source future scripts/probes/qa-f8-close/**, aucun symbole créé/présence statique fictive ; pool8 reste seul stdio.

### Risques QA distincts du risquePM

- R1 enfantvivant/cleanupprematuré : **I4×P3=12**. Déclencheurdetachbloqué,timeoutattente/transportexterne,supérationperdue ; blast radius processus/fixtures/handles possédés/capacitérunner. Détection documentaire50 et wrapper54, pas runtime. Mitigation attentehandle, supervision/bailconservés,pascleanup/kill ; **gapbloquant superviseurdurable/conduiteaprèsdélai** avantauteur.
- R2 ownershipconsommé : **I4×P3=12**. Déclencheur garde/atexit/réemploiFILE/fd/valeurparent/retraitflag+CloseHandle ; blast radius doubleclose/fdrecyclé/ressourceétrangère au processus. Détection statique42/47/50 ; mitigation étatconsommé/zeroréemploi et revuecheminsprécoces ; **gapcontratdestruction/exception** avantauteur, consommationnonaffirmée si exceptionnative interromptfclose.
- R3 fauxverdictCRT/syscall/Reader : **I3×P4=12**. DéclencheurSDKrcprisDLL,errnopréchoisi,EOFprisCloseHandleobservé,suppressionprisefermeture,calibrationprisepublication. Blast radius crédibilitéE4/décisionmigration. Mitigation modulesréellementchargés/hash/étapes et verdictsparniveau ; **gap mécanismeobservationFILE/exception etidentitéCRT**. Pas nouveauxtraceursyscallexigé : refusinterne reste documentaire/inféré sanstrace, nepasétiqueterexecuted. Pas installation/service/programmetracing pourremplircase.

PrioriserrevueR1/R2 avantcalibrationR3 ; réévaluerauxsurprises/changementcandidat sanssweep/campagneGL/ASan. Churninfluenceprobabilitépasimpact, scoresPM61/73 indépendantsdecettematrice.

### Scénarios et oracles

- C0 nominal : enfantdistinct/fixturelectureseule,lecturecomplèteavantclose,protectionabsente,une fclose0 à observer,résultatimmédiat,puisfinprocesssignal/statut/logsconcordants. Parentfermeseulesseshandles/cleanupaprèsfin,voisinsintacts ; exit0seulinsuffisant.
- C1 cible : mêmebinaire/config/données,empruntvalide/protectionappliquéeetlueavantcallunique. Phases lecturefinie→protectionconfirmée→entrée fclose→retourOUexception→fin. Capturerretour/errnoavantlogging ; GetLastErroraprèsCRTneprouvepasCloseHandleinterne. AucunCloseHandletémoin surl'emprunt. Sansobservationinternequalifier FILE*/politiqueconfigurée, pas syscallobservé ; pascodes/absenceexceptionprédéterminés.
- C2 setup : création/open/lecture/instrumentationéchouée avantconditioncomplète→arrêtbras,pascalibrationclose. Nettoyeruniquementacquis/vivantsconformeowner ; lectureéchouée≠closeéchoué, instrumentationéchouée≠permissionderefermeremprunt.
- C3 retour/exception : aprèsretourconsommant,pasusageFILE/fd,logsseulspuissortie ; exceptionnative/debuggerneprouve ni retour ni consommationachevée, jamaishandler/retryinventantcettepreuve. Finprocessrécupérationdocumentée50≠tracechaquehandlekernel.
- C4 timeout/WAIT_FAILED/logsincomplets/crash : étatidentités/bailconservés,pasautrebras/rerunsurlamêmeressource,nicleanupavantfin. Maintenirsupervisionoutransfertautoriséexplicite ; plusvivantque budget=INCONCLUSIVE/décisionopérateur,pasrecoveryPASS. Ne pas provoquerdeadlockpourcouvrirligne. AttentebornéeWaitForSingleObject/signalavantGetExitCodeProcess ; WAIT_FAILEDdistincttimeout, derniercodeapparentnelibèrepaspropriété.
- C5 cleanupaprèsfin : suppressionracinepossédéeetvoisins ; échec=FAILcleanup, preuveclosedistinctesansPASSglobal. Suppressionseuleinsuffisante(share-delete). Sortiesprécocesconserventresponsabilitésupervisionvivante.

NouveauxcontrôlesdéterministessuiventvraisRED/GREENsi défaut/capacitémanquantepertinents ; pascodevolontairementfaux. RésultatC1inconnu=caractérisation,pasREDparcequefcloseéchoueintentionnellement. Aucune duplicationwhole-native-readS. Si branchetimeout/setupnécessiteinfrastructuresupplémentairenonadoptée : revue statique+GAP, passeam/mockimplicitement. FuturcorrectifReadercontrat/REDséparés.

### Registre, transport et sortie

E4-calibration→futurecloseC0/C1(GAPruntime) ; I-lifetime→42fclose17–43/close81–94 et50ownership(stat/digestionlead) ; I-identité→42limiteSDK/futurmanifestemodules(GAP) ; I-supervision/exclusivité→50detach/52conditions,C2–C5(GAPprotocole) ; E4Reader→readShaderFileclose/publication/M4catch(NONcouvert).

Inverse42/43→hypothèseseeknonsoutenuepasimpossibilité ;47/50→contratspublicspasrun ;52→admissibilitépaspreuve ; futurslogsCRT/signal/cleanup→C1/C3/C4/C5 séparément,paspublication. Close nominal≠refus,EOF/errno≠traceOS,résumé/log/XML≠3runs ; orphelinsunassessedavantdiff,ASan555autrecandidatpaspreuveclose. Source/datelectures26octobre,commandes0/runneraucunpourQA ; identitéDLL/binaire/delta/logs/env/commandeexacte/attenduobservé/comptes àfixeravant/aprèsrunner, paszérotestPASS.

Findingtransportdigestionlead : run-build.py utilise subprocess.run(timeout=1200),tueenfantdirectsurTimeoutExpired ; parentclose ainsi tuépourrait laisserenfantorphelin. **Compilewrapperréutilisable, RUNcloseinterditviaCEwrapperpardéfaut.** Gateavant auteur : transportRUNsanskillimplicite,bailresponsable/PID+instantcréation/bras/handlesvivants,markers/logs/observabilitéaprèsdélai etsuiviaulead. Markerne remplacepas signalhandle ; deadlineinterne/externe≠fin,aucunréemploibail/fixturetantquenonfin. Manque de transport sûr=BLOCKED, pasforcekill/service/infra supplémentaireinventés. LignesRUNàvalider parleadavantessai.

Budgetlocalproposé100unités(nonheures/coût,nonsumautomatiqueglobal17) : setup30 ; reste70dont52,5planifié75%et17,5buffer25%. Planifié supervision/lifetime14,nominal/setup12,refus/observation12,cleanup8,registre/revue6,5 ; bufferinvestigation7/panne6/réaudit4,5. Si transportadmissibleabsent/enveloppetenableabsente : arbitrageavantauteur,pasréductionrisquehautoubuffer.

Sortiepréparation=ajout+protocolecommisavantfreshsource,ownershipséparé/unbuildsortieleadrunner ; **cetajoutn'affirme pas auteurprêt** tantquesupervision/observation/identiténonarrêtées. Sortiecalibration=brasidentifiés/résultatsfidèles/finconstatée/cleanupvérifié,sinonFAILécartouINCONCLUSIVEmanque. M5Reader/catchGAP,aucuneGL/ASan/Linux/GCC/Release/image/perf/PASSglobal implicite.

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
