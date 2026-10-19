# Transcript verbatim — session ses_eace9372affeVmG65TfK8MtKUE

- **Titre** : Valider le plan mission OpenGL
- **Agent** : project-manager | **Modèle** : {"id":"glm-5.3-flash","providerID":"opencode-go","variant":"max"}
- **Parent** : ses_eacea7ee9ffdJ6glP3t5iNOc95 | **Fork de** : -
- **Répertoire** : C:/Users/djden/source/repos/ARTIST
- **Créée** : 2026-10-19 07:36:04.768Z | **Messages** : 10
- **Export** : 2026-10-19T11:41:55.916Z

---

### [4] user @ 2026-10-19 07:36:04.847Z

You are a subagent spawned by another session.
Tu es planificateur en lecture seule pour ARTIST. Ne modifie aucun fichier, ne crée aucune équipe, ne lance aucun build. Ton livrable est un plan validé sous forme de texte.

## Mission à préparer

Objectif : « Rendre déterministes les frontières de création, remplacement, refus et libération des ressources OpenGL existantes, sans reconstruire le moteur » — quatre branches d'auteur (cycle VBO Attribute, publication réflexion Pass, refus Uniform, isolation fixtures Loader), exécution long_run après approbation native.

Le cadrage complet est dans `C:\Users\djden\source\repos\ARTIST\docs\architecture\next-mission.md` (commit HEAD cd1b598 = « docs: propose parallel OpenGL reliability long-run mission »). LIS ce fichier en entier : il contient le graphe candidat produit par deux PM, les scores, les décisions K à fermer, le budget et les preuves attendues. Ton travail est de VALIDER ce candidat et de produire les fiches prêtes à publier, pas de réinventer le découpage.

## État du dépôt (vérifié à l'instant)

- HEAD cd1b598 ; les fichiers dirty externes sont préservés et hors périmètre (ne pas les toucher, ne pas exiger leur commit) : .grepai/config.yaml, .opencode/agents/{orchestrator,qa-planner,test-engineer,test-reviewer}.md, .opencode/ensemble.json, .opencode/evidence/qa-tools-policy-2026-10-18/, AGENTS.md, docs/agentic/gpu-qa-harness.md, docs/architecture/correction-plan.md, docs/index.md, docs/log.md, + missions historiques team_mvdpbghz*/team_mve3tz7l*/team_mve4mmol* non rattachées à la session.
- Un futur worktree ne recevra pas ce dirty implicitement.

## Chemins réels vérifiés sur disque (corrections à intégrer aux fiches)

Le doc de mission abrège certains chemins. Vérifié par glob/grep sur ARTIST/include :
- `ARTIST/include/graphic/pipeline/Attribute.hpp` existe (branche A).
- `ARTIST/include/graphic/opengl/pipeline/component/attribute/Binder.hpp` existe (A).
- `ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp` existe (B) — publie via `openglContext.addAttribute` (L100).
- `ARTIST/include/graphic/opengl/pipeline/component/pass/UniformReader.hpp` existe (B) — publie via `openglContext.addUniform` (L97).
- Le « context/PassContext.hpp » de la mission = `ARTIST/include/graphic/context/PassContext.hpp` (addUniform L51, addAttribute L67) ET son sous-classe `ARTIST/include/graphic/opengl/context/PassContext.hpp` (`OpenGLPassContext : public graphic::context::PassContext<OpenGL>`). La branche B doit donc posséder les DEUX fichiers PassContext (base + GL), car la logique de remplacement d'owner est dans la base et le chemin de publication passe par la sous-classe.
- Le « pipeline/Uniform.hpp » de la mission = `ARTIST/include/graphic/pipeline/Uniform.hpp` (class Uniform L51). Le « contexte Uniform » couvre `ARTIST/include/graphic/context/UniformContext.hpp` et `ARTIST/include/graphic/opengl/context/UniformContext.hpp` (tous deux existent). Branche C.
- `ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp` existe (C).
- `ARTIST/tests/mock/opengl/glFunctionMock.hpp` existe (M éventuel).
- `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/LoaderTests.cpp` et `ARTIST/tests/src/graphic/opengl/pipeline/component/shader/LoaderTests.cpp` existent (H).

Vérifie toi-même par grep/glob les points que tu juges nécessaires pour valider les ownership et détecter les recouvrements entre branches (par exemple : qui inclut glFunctionMock.hpp, où vivent les tests Attribute/Uniform cités, si `graphic/opengl/context/UniformContext.hpp` est inclus par les Readers). Lecture seule.

## Contrat d'évaluation (obligatoire)

- Mode PM/LLM par défaut, retenu pour cette mission (pas de JEV sans choix explicite). Pour chaque tâche : estimation brute → retenue, complexité ET risque sur 100, heuristiques indépendantes, avec justification courte et incertitudes.
- Développement, code de tests et harness : complexité retenue = max(70, brute). Ne jamais abaisser le risque pour obtenir un modèle. Le risque PM (0–100) est distinct de QA I×P (1–5 → 1–25).
- Contexte fresh pour chaque fiche ; lecteurs RO sans worktree ; auteurs isolés sur leurs fichiers.

## Ce que je te demande de livrer

1. Les fiches de tâches validées/corrigées : id (P, K, Q, M, A, B, C, H, I, G, F, R, V), agent propriétaire, fichiers/symboles owned avec les chemins réels ci-dessus, dépendances (vraies barrières d'intégration, pas « fin d'auteur »), scores brute→retenue C/R validés ou corrigés, contexte, critères observables et preuves RED/GREEN attendues. Corrige les scores si le candidat du doc te semble mal étalonné, en justifiant.
2. Le graphe d'exécution avec les barrières de publication : quels consommateurs restent hors board jusqu'à l'intégration réelle des producteurs (A/B/C/H jusqu'à I ; G/R/F/V jusqu'à I ; V après candidat final intégré).
3. Le déclencheur QA : confirmer que risque PM ≥ 50 et comportement sous contrat imposent qa-planner (Q) avant tout auteur, plan persisté/commité avant leurs worktrees, puis test-engineer (G) après I et test-reviewer fresh (V) hors board d'implémentation. QA I×P pour les produits ≥ 10 avec déclencheur/blast radius/détection/mitigation/gap.
4. Les décisions K à fermer avant les auteurs (reprendre les 5 du doc), en précisant pour chacune ce qui bloque les branches et ce qui reste une décision humaine/lead.
5. Les écarts ou risques que tu détectes entre le doc et le code réel (chemins, ownership, tests existants), avec preuve (chemin:ligne).
6. Une confirmation explicite des points restant en préparation seule : aucune publication, aucun worker, aucune écriture de source — le cycle natif /ensemble-mission sera initié par l'utilisateur.

Format de sortie : plan compact en français, fiches numérotées, graphe en texte, puis écarts et risques. Pas de dump de fichiers.

### [5] assistant @ 2026-10-19 07:36:04.871Z — opencode-go/glm-5.3-flash/max

**reasoning** :

Let me start by reading the mission document and verifying the paths. I'm a read-only PM planner. Let me read the full mission doc first, then verify key facts in the codebase via grep/glob.

Let me begin.

Je lis d'abord le document de cadrage en entier, puis je vérifierai les points d'ownership par grep/glob.

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\docs\\architecture\\next-mission.md"
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\docs\\architecture\\next-mission.md, lines 1-291\n1: ---\n2: title: \"Prochaine mission — fiabilité et lifetime des ressources OpenGL\"\n3: category: architecture\n4: tags: [artist, architecture, rendering, testing]\n5: aliases: [next-mission]\n6: summary: \"Proposition long run : VBO Attribute, publication de réflexion, refus Uniform et fixtures Loader, en branches parallèles après contrats et QA.\"\n7: status: proposed\n8: implementation: not-started\n9: source_revision: 070e20d9157b9d90978e51053a59f843c0e93250\n10: created: 2026-10-18\n11: updated: 2026-10-19\n12: sources:\n13:   - \"opencode:ses_eacf31fbaffeam2skOnm89Pi66\"\n14:   - \"opencode:ses_eacf2ea5affeBDZfFmS3E05oRl\"\n15:   - \"references/session-2026-10-18-attribute-gl.md\"\n16: provenance:\n17:   extracted: 0.5\n18:   inferred: 0.5\n19:   ambiguous: 0.0\n20: ---\n21: \n22: # Prochaine mission — fiabilité et lifetime des ressources OpenGL\n23: \n24: > [!important] Proposition, pas exécution\n25: > Mission relativement large, prévue pour une exécution autonome **long_run**\n26: > après préparation et approbation natives. Aucun auteur, test RED, build ou\n27: > changement moteur lancé pour ce cadrage. Les décisions ouvertes ci-dessous\n28: > empêchent encore de publier les tâches d'implémentation.\n29: \n30: ## Origine et état réel\n31: \n32: L'[[architecture/missions/2026-10-19-attribute-session-ingestion|ingestion de\n33: session Attribute-GL est archivée]]. Elle n'a pas adopté de garde-fous ni fermé\n34: le chantier moteur. La [[references/session-2026-10-18-attribute-gl|rétrospective]]\n35: conserve les décisions et rectifications, dont le retrait des sondes purement\n36: driver. Ne pas reproposer l'implémentation Attribute-GL déjà intégrée.\n37: \n38: Les deux PM ont enquêté en lecture seule sur `070e20d`. Leurs avis ne comportent\n39: aucun nouveau run :326PASS Windows Debug (266CPU+60GL) est une preuve historique\n40: du retrait Q3 àf7bb171, pas la baseline exécutée de cette future mission.\n41: Le pointeur Attribute antérieur, y compris son diff documentaire préexistant,\n42: est conservé intégralement dans Git :\n43: `28395ff16f9026522f593a012396bfe915264971:docs/architecture/next-mission.md`.\n44: Ne pas le lire comme une description actuelle des défauts à corriger.\n45: \n46: Dirty AGENTS/profils/harness/index/log/correction-plan/.grepai et politiques\n47: externes préservés. L'autorisation présente porte sur ce pointeur et l'archive,\n48: pas sur ces autres fichiers. Un futur worktree ne reçoit pas le dirty implicitement.\n49: \n50: ## Résultat attendu et bornage\n51: \n52: **Rendre déterministes les frontières de création, remplacement, refus et\n53: libération des ressources OpenGL existantes**, sans reconstruire le moteur.\n54: Quatre branches d'auteur fournissent une enveloppe long run significative :\n55: \n56: 1. Fermer le cycle des VBO créés pour Attribute, selon un contrat de contexte\n57:    courant et d'ownership explicitement adopté.\n58: 2. Définir puis respecter la frontière de publication de la réflexion Pass :\n59:    succès, échec tardif, remplacement, noms disparus et emprunts invalidés.\n60: 3. Définir puis respecter les refus Uniform et l'état CPU observable après refus,\n61:    sans inventer une compatibilité GLSL universelle.\n62: 4. Requalifier et, si nécessaire, isoler les fixtures Loader/ShaderLoader sous\n63:    permutations, sans masquer les erreurs de production.\n64: \n65: La largeur vient de frontières testables et de fichiers disjoints, pas de l'ajout\n66: de fonctionnalités sans oracle. Chaque branche peut aboutir à un constat sans\n67: patch si sa suspicion ne se reproduit pas. Ne pas fabriquer un RED pour remplir\n68: le budget ni présenter la publication partielle historique comme un bug adopté.\n69: \n70: ### Observations statiques transmises par les PM\n71: \n72: - `ARTIST/include/graphic/opengl/pipeline/component/attribute/Binder.hpp` crée et\n73:   publie un VBO ; aucun glDeleteBuffers n'a été trouvé dans ARTIST/include lors\n74:   de leur enquête. Attribute a destructeur/moves par défaut. Ce constat ne décide\n75:   pas encore qui doit supprimer le nom, quand ni sous quel contexte.\n76: - Les Readers `pass/AttributeReader.hpp` et `pass/UniformReader.hpp` publient dans\n77:   leur boucle ; `PassContext::addAttribute/addUniform` remplace immédiatement\n78:   l'owner. Une publication partielle est déjà caractérisée par des tests.\n79: - `Uniform::set` stocke la valeur avant le Setter ; un refus de cardinalité peut\n80:   suivre. Le contrat ne promet actuellement pas partout un rollback CPU. Les\n81:   compatibilités ne se réduisent pas à l'égalité naïve de deux enums.\n82: - Les incidents shuffle Loader/ShaderLoader sont historiques, non reproduits sur\n83:   ce candidat pendant le cadrage. La lecture fichier Shader a déjà été corrigée :\n84:   ne pas la reproposer comme neuve.\n85: \n86: Ces constats sont des pistes à contractualiser, **pas des bugs reproduits par\n87: cette préparation**. GrepAI et Serena ont aidé les PM ; une recherche symbolique\n88: Loader vide a été complétée textuellement. La fiabilité exhaustive de leur analyse\n89: C++ sans base de compilation complète n'est pas démontrée.\n90: \n91: ### Hors périmètre\n92: \n93: Nouveaux backends, render graph, assets/cooker, handles générationnels, gestionnaire\n94: global de contexte, retraite GPU générale, RAII exhaustive, refonte de Pass::load\n95: implicitement transactionnel, support générique des types, perf/zero-cost, image\n96: de référence, portage OS et réparation générale du harness agentique.\n97: Pas de glFinish par Attribute pour masquer un contrat de complétion absent.\n98: Le nettoyage Shader avant publication, autre suggestion PM, reste une tranche\n99: distincte à préparer seulement si un RED causal est établi ; il n'est pas ajouté\n100: comme cinquième branche pour gonfler artificiellement le parallélisme.\n101: \n102: ## K — Décisions à fermer avant auteurs\n103: \n104: Principal `engine-architect` ; renfort ressources seulement pour le ledger natif.\n105: Le spécialiste OpenGL reste `gpu-engineer` pour l'implémentation du backend.\n106: \n107: 1. **VBO :** owner/adoption/emprunt de setBufferID, rebind, release explicite ou\n108:    automatique, exact-once, move chargé→chargé, destruction/remplacement en map,\n109:    état moved-from, contexte/thread et cas sans contexte. Références VAO et\n110:    suppression logique ne prouvent pas une retraite physique GPU générale.\n111: 2. **Réflexion :** atomicité par Reader ou Pass complet, ancien état conservé ou\n112:    effacé en échec, collection vide/noms disparus et moment d'invalidation des\n113:    emprunts. Tout changement de garantie publique requiert décision et oracle.\n114: 3. **Uniform :** état CPU après rejet, matrice CPU/GLSL admise (bool/samplers/\n115:    entiers inclus), programme courant et doubles/extensions. Aucun élargissement\n116:    ni retrait silencieux du contrat existant.\n117: 4. **Interactions :** responsabilité de PassContext lors du remplacement d'un\n118:    Attribute désormais owner VBO ; interfaces de métadonnées stables entre B/C.\n119:    Séparer les fichiers auteur, ou sérialiser la frontière si elle ne se ferme pas.\n120: 5. **Dette Attribute-GL :** relever l'audit final encore ouvert ; établir avant\n121:    mutation un candidat et une baseline courants. Tout FAIL causal dans les mêmes\n122:    frontières est traité ou arbitré avant consommation, jamais absorbé par326PASS.\n123: \n124: Le lead persiste les options et la décision humaine préalable sur ces garanties.\n125: Si elles imposent une architecture globale, K s'arrête et rend le problème au\n126: lead : long_run n'autorise pas une refonte silencieuse ni une décision produit.\n127: \n128: ### Binding times\n129: \n130: Traits/layouts/dispatch connus par les types : compile-time. Réflexion et\n131: validation du programme réellement lié : préparation runtime après link GLSL.\n132: Identités GL, contexte/thread, valeurs, borrows et transitions d'owner : runtime.\n133: Aucun nouveau contrat cook ou link-time C++ n'est présumé.\n134: \n135: ## Graphe exécutable à préparer, non publié\n136: \n137: Évaluation PM/LLM : **C brute→retenue / R** sur100, heuristiques indépendantes.\n138: Développement, code de tests et harness : C retenue=max(70,brute). Risque jamais\n139: modifié pour obtenir un modèle. Contexte fresh pour chaque fiche ; lecteurs RO\n140: sans worktree, auteurs isolés. Aucun modèle/provider/variante imposé par ce plan.\n141: Les références ci-dessous ne sont pas des IDs Ensemble.\n142: \n143: - **P — entrée documentaire**, lead, **30→30/35**. Archive réalisée et pointeur\n144:   proposé ; pas de scope moteur commencé. Docs canoniques hors ownership restent\n145:   intactes. Le HEAD intégré sera transmis à K/Q.\n146: - **K — contrats**, engine-architect RO, **76→76/78**. Ledger, états, erreurs,\n147:   alternatives et décisions ci-dessus, sans source modifiée. Puis validation lead/\n148:   humaine. Un contrat incompatible avec l'autonomie bloque les auteurs.\n149: - **Q — plan QA**, qa-planner RO, après K validé, **60→60/62**. Exigences\n150:   explicites/inférées, nominal/négatif, invariants, top3 risques, preuve↔obligation\n151:   et orphelins ; QA I×P(1–5), champs déclencheur/blast radius/détection/mitigation/\n152:   gap pour tout produit≥10. Lead persiste et commit avant TOUT auteur.\n153: - **M — éventuelles entrées mock communes**, cpp-engineer, **45→70/58**\n154:   (provision lead pour résoudre un conflit d'ownership détecté entre les plans).\n155:   Seulement après K/Q committés ; propriétaire unique de\n156:   `ARTIST/tests/mock/opengl/glFunctionMock.hpp` si une extension est nécessaire.\n157:   RED/GREEN local au double, comportements historiques préservés ; pas le RED\n158:   métier des auteurs. Borner/réévaluer cette fiche après K ; si inutile, pas de\n159:   tâche factice. Intégrer M avant ses consommateurs, pas en parallèle avec eux.\n160: - **A — cycle VBO**, gpu-engineer, **82→82/86**. Seul propriétaire de\n161:   `graphic/pipeline/Attribute.hpp`, contexte Attribute, composants `attribute/`,\n162:   tests Attribute associés et nouveau fichier GL lifetime. Pas de PassContext,\n163:   Uniform, Readers ou mock commun. RED exact-once/move/rebind/release selon K,\n164:   GREEN minimal et régression des12formes/Pointer/IPointer/VBO propre.\n165: - **B — publication réflexion**, cpp-engineer distinct, **78→78/80**. Seul\n166:   propriétaire de `context/PassContext.hpp`, Readers `pass/AttributeReader.hpp`\n167:   et `pass/UniformReader.hpp`, tests Readers et fichier d'intégration dédié.\n168:   Pas de contexte Attribute/Uniform ni de mock commun. RED première entrée valide\n169:   puis seconde invalide avec ancien état identifiable ; succès/échec/vide/noms\n170:   disparus selon K. Ne pas affaiblir le test de publication historique : tout\n171:   changement est un amendement explicite de contrat.\n172: - **C — refus Uniform**, autre cpp-engineer, **68→70/72**. Seul propriétaire de\n173:   `pipeline/Uniform.hpp`, contexte Uniform, `uniform/Setter.hpp`, tests de valeurs/\n174:   cardinalité/compatibilité et nouveau fichier GL dédié. Pas de Reader ni de mock\n175:   commun. RED via API publique, ancienne valeur observée selon la garantie K,\n176:   zéro upload au refus ; succès par famille réellement autorisée.\n177: - **H — isolation fixtures**, build-engineer, **56→70/55**. Seulement les deux\n178:   `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/LoaderTests.cpp` et\n179:   `ARTIST/tests/src/graphic/opengl/pipeline/component/shader/LoaderTests.cpp`,\n180:   et helper neuf local si nécessaire. Chemins confirmés sur disque pendant ce\n181:   cadrage. Pas de Loader production, CMake ou\n182:   mock global. Seeds historiques et permutations inversées sur candidat identifié,\n183:   contrôle négatif conservé ; sinon caractérisation sans patch artificiel.\n184: - **I — intégration et interactions**, lead avec propriétaires, **64→70/76**.\n185:   A/B/C/H réellement shutdown/merge, diff relu puis tests croisés remplacement\n186:   VBO/réflexion et Uniform réfléchi/upload. Tout code de résolution retourne à son\n187:   owner ; toute nouvelle fixture ou adaptation reçoit ownership et tranche TDD.\n188: - **G — gaps**, test-engineer RO, après I, **58→58/60**. Propositions texte avec\n189:   oracles et KEEP/MODIFY/REJECT/DEFER ; pas de production ni runner autonome.\n190:   Il complète les auteurs, ne fournit pas leur RED. Le lead persiste et exécute.\n191: - **F — correction des gaps acceptés**, provision **62→70/65** pour code de test.\n192:   Ne pas publier une tâche générique : chemins, owner et scores réévalués sur les\n193:   vrais gaps de G ; correction production renvoyée à A/B/C/H. Nouvelle intégration.\n194: - **R — revue lifetime**, correctness-reviewer fresh RO, après I,\n195:   **66→66/78** (estimation PM indépendant). Parallèle à G ; findings corrigés par\n196:   propriétaires, invalidation des conclusions affectées après mutation.\n197: - **V — audit final**, test-reviewer fresh HORS board d'implémentation,\n198:   **70→70/77**. Candidat final intégré, contrat et preuves seulement, pas les\n199:   justifications auteur. Scénario discriminant avant finding haut, puis retour\n200:   propriétaire/réintégration/ré-audit. Rendering-verifier en renfort si une preuve\n201:   GPU reste ambiguë ; pas de nouveau coordinateur.\n202: \n203: ```text\n204: P → K → décisions → Q → commit contrats/QA → [M intégré si nécessaire]\n205:                                            ↓\n206:                                      A || B || C || H\n207:                                            ↓\n208:                                    I → G || R → corrections bornées\n209:                                            ↓\n210:                                   candidat final intégré → V fresh\n211:                                            ↓\n212:                                   validation lead → documentation\n213: ```\n214: \n215: La rédaction P est indépendante des investigations K. A/B/C/H sont parallèles\n216: seulement après gel des interfaces et intégration des entrées communes. Si le\n217: contrat révèle un fichier commun restant, un seul owner et une nouvelle barrière.\n218: Le lead garde les consommateurs hors board jusqu'aux vraies intégrations ; pas\n219: de tâche factice d'intégration, pas de dépendance sur la simple fin d'auteur.\n220: \n221: ## Exécution autonome et ressources\n222: \n223: Une fois les contrats, QA et politique approuvés, le coordinateur peut enchaîner\n224: les tâches prêtes, corrections bornées et validations sans redemander chaque\n225: détail local. Escalade obligatoire pour contrat/portée ouverts, accès manquant,\n226: préférence modèle différente, FAIL critique ou deux essais sans information\n227: nouvelle. Healer QA au plus deux réparations ; bug production reste rouge.\n228: Une pause opérateur exige son GO distinct ; long_run n'autorise pas à la lever.\n229: \n230: Un écrivain par fichier. Les écritures A/B/C/H sont parallèles ; les builds sont\n231: sérialisés par arbre de sortie, même si les auteurs sont indépendants. Un seul\n232: bail GPU/campagne GL ; processus partageant fixtures/singleton mock sérialisés.\n233: Un indexeur QMD canonique, lead runner pour les propositions QA. Pas de mélange\n234: des preuves de branches ni d'arrêt d'une ressource externe au lot.\n235: \n236: ## Preuves et conditions de sortie\n237: \n238: Avant tests, charger artist-build-validation et vérifier les préconditions réelles.\n239: Commandes futures à adapter au build identifié, non exécutées pour ce cadrage :\n240: \n241: ```text\n242: scripts/build-windows.ps1 -Configuration Debug -TestLabel unit -Jobs 1\n243: ctest --test-dir <build> -C Debug -N --show-only=json-v1\n244: ctest --test-dir <build> -C Debug -L unit --no-tests=error --output-on-failure --parallel 1\n245: ctest --test-dir <build> -C Debug -L opengl --no-tests=error --output-on-failure --parallel 1\n246: ```\n247: \n248: Reconfigurer après ajout de sources, vérifier découverte non nulle et nouveaux\n249: tests présents ; shuffle sur le binaire unit identifié. ASan seulement après\n250: préflight, arbre séparé, canari et sélection documentés ; absence de preuve reste\n251: INCONCLUSIVE, sans héritage du résultat historique. Ne pas supprimer une erreur\n252: de production en corrigeant une fixture ni ajouter de sonde de conformité driver.\n253: \n254: RED causal de chaque branche avant implémentation, GREEN minimal puis refactor/\n255: non-régression ; environnement indisponible et zéro test ne sont jamais RED/PASS.\n256: Mocks prouvent arguments/absence d'appels ; GL réel prouve les obligations natives\n257: retenues. glIsBuffer seul ne prouve ni ownership exact-once ni retraite physique.\n258: Tous les cas création/release/move/remplacement, échec tardif de réflexion et refus\n259: Uniform du contrat K/Q ont un verdict attribué au candidat final. Rapport : HEAD,\n260: delta/hashes, commandes, environnement, comptages, skips et limites. Aucun PASS\n261: global tant qu'un critère requis est FAIL ou INCONCLUSIVE.\n262: \n263: ### Budget proposé, pas durée promise\n264: \n265: PM principal : setup5–9 h-personnes ; travail productif32–50 h-personnes ;\n266: buffer11–17 h supplémentaires, soit environ75% planifié/25% buffer après setup.\n267: Ces nombres ne sont ni un SLA ni une durée murale ; les runs GPU/build et le\n268: chemin K→Q→A/B→I limitent le gain du parallélisme. Réévaluer après K et M ;\n269: ne pas consommer le buffer pour élargir la portée. Couper le confort avant un\n270: oracle haut risque et ne jamais financer une branche sans cause établie.\n271: \n272: ## Adoption et démarrage futurs\n273: \n274: Le lead retient ce candidat large du PM principal. L'avis indépendant privilégiait\n275: un lot VBO+nettoyage Shader plus étroit ; sa réserve est conservée : si K ne peut\n276: fermer réflexion/Uniform sans refonte globale, réduire au cycle VBO et rendre les\n277: autres garanties au décideur, plutôt que poursuivre implicitement.\n278: \n279: La présente demande autorise **la préparation documentaire seulement**. Préparer\n280: une nouvelle mission native avec cet objectif, puis demander à l'utilisateur :\n281: `/ensemble-mission config --mission-mode long_run`, présentation config de la\n282: politique complète et `/ensemble-mission start`. Ne pas invoquer start à sa place,\n283: réutiliser l'approbation de l'ingestion ou choisir des modèles hors grille.\n284: Avant publication future : require_context, deux scores, affinités et vrais IDs\n285: des producteurs intégrés ; aucune classification JEV sans choix explicite.\n286: \n287: ADR0003/0005 Accepted/partial,0008 Windows Debug seulement ;0001/2/4/6/7 Proposed.\n288: K doit adopter explicitement toute garantie nouvelle sans promouvoir les ADR\n289: globaux par accident. Voir [[adr/README|le registre]] et\n290: [[architecture/correction-plan|l'ordre de correction]], dont le pointeur Attribute\n291: antérieur reste un état documentaire historique, non corrigé dans un dirty externe."
    }
  ],
  "metadata": {
    "truncated": false
  }
}
```

### [16] assistant @ 2026-10-19 07:36:08.064Z — opencode-go/glm-5.3-flash/max

**reasoning** :

The doc is fully read. Now I need to verify the ownership/overlap points via grep/glob. Key things to verify:

1. Who includes glFunctionMock.hpp — to detect overlap between branches (A/B/C/H/M).
2. Where do Attribute/Uniform tests live — to verify branch ownership of tests.
3. Is `graphic/opengl/context/UniformContext.hpp` included by the Readers — overlap between B and C.
4. Verify the paths listed: Attribute.hpp, Binder.hpp, AttributeReader.hpp, UniformReader.hpp, PassContext (base + GL), Uniform.hpp, UniformContext (base + GL), Setter.hpp, glFunctionMock.hpp, LoaderTests.cpp ×2.
5. Check whether glDeleteBuffers exists anywhere in ARTIST/include (validate observation).
6. Check which tests reference AttributeReader/UniformReader — overlap of B's test ownership with A's or C's.
7. Check if tests for Attribute (12 formes/Pointer/IPointer/VBO) exist.
8. Check PassContext::addAttribute/addUniform ownership replacement in base.
9. Check if OpenGLPassContext includes/uses UniformContext.

Let me batch greps and globs. I'll use the native grep/glob tools since they're available directly (matching exact patterns is fine with native tools per the harness rules; also this is read-only). Actually the AGENTS.md says to use GrepAI for understanding code, but for exact matching (file names, includes), native grep/glob is appropriate. I'll do a mix — but simplest: batch of native greps/globs in parallel.

Let me run them.

Document lu intégralement. Je vérifie maintenant les points d'ownership et recouvrements dans le code réel (lectures en parallèle).

**outil `grep`** :

```json
{
  "status": "completed",
  "input": {
    "pattern": "glFunctionMock",
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST",
    "limit": 40
  },
  "content": [
    {
      "type": "text",
      "text": "Found 40 matches\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\mock\\opengl\\glFunctionMock.hpp:\n  Line 2:  * @file glFunctionMock.hpp\n\n  Line 8:  * the `artist::mock::opengl::glFunctionMock` class.\n\n  Line 39: #define glUniform1f artist::mock::opengl::glFunctionMock::instance()->glUniform1f_mock\n\n  Line 40: #define glUniform1d artist::mock::opengl::glFunctionMock::instance()->glUniform1d_mock\n\n  Line 41: #define glUniform1i artist::mock::opengl::glFunctionMock::instance()->glUniform1i_mock\n\n  Line 42: #define glUniform1ui artist::mock::opengl::glFunctionMock::instance()->glUniform1ui_mock\n\n  Line 43: #define glUniform2fv artist::mock::opengl::glFunctionMock::instance()->glUniform2fv_mock\n\n  Line 44: #define glUniform3fv artist::mock::opengl::glFunctionMock::instance()->glUniform3fv_mock\n\n  Line 45: #define glUniform4fv artist::mock::opengl::glFunctionMock::instance()->glUniform4fv_mock\n\n  Line 46: #define glUniformMatrix2fv artist::mock::opengl::glFunctionMock::instance()->glUniformMatrix2fv_mock\n\n  Line 47: #define glUniformMatrix3fv artist::mock::opengl::glFunctionMock::instance()->glUniformMatrix3fv_mock\n\n  Line 48: #define glUniformMatrix4fv artist::mock::opengl::glFunctionMock::instance()->glUniformMatrix4fv_mock\n\n  Line 49: #define glUseProgram artist::mock::opengl::glFunctionMock::instance()->glUseProgram_mock\n\n  Line 50: #define glValidateProgram artist::mock::opengl::glFunctionMock::instance()->glValidateProgram_mock\n\n  Line 51: #define glAttachShader artist::mock::opengl::glFunctionMock::instance()->glAttachShader_mock\n\n  Line 52: #define glBindAttribLocation artist::mock::opengl::glFunctionMock::instance()->glBindAttribLocation_mock\n\n  Line 53: #define glBlendEquationSeparate artist::mock::opengl::glFunctionMock::instance()->glBlendEquationSeparate_mock\n\n  Line 54: #define glCompileShader artist::mock::opengl::glFunctionMock::instance()->glCompileShader_mock\n\n  Line 55: #define glCreateProgram artist::mock::opengl::glFunctionMock::instance()->glCreateProgram_mock\n\n  Line 56: #define glCreateShader artist::mock::opengl::glFunctionMock::instance()->glCreateShader_mock\n\n  Line 57: #define glDeleteProgram artist::mock::opengl::glFunctionMock::instance()->glDeleteProgram_mock\n\n  Line 58: #define glDeleteShader artist::mock::opengl::glFunctionMock::instance()->glDeleteShader_mock\n\n  Line 59: #define glDetachShader artist::mock::opengl::glFunctionMock::instance()->glDetachShader_mock\n\n  Line 60: #define glDisableVertexAttribArray artist::mock::opengl::glFunctionMock::instance()->glDisableVertexAttribArray_mock\n\n  Line 61: #define glDrawBuffers artist::mock::opengl::glFunctionMock::instance()->glDrawBuffers_mock\n\n  Line 62: #define glEnableVertexAttribArray artist::mock::opengl::glFunctionMock::instance()->glEnableVertexAttribArray_mock\n\n  Line 63: #define glGetActiveAttrib artist::mock::opengl::glFunctionMock::instance()->glGetActiveAttrib_mock\n\n  Line 64: #define glGetActiveUniform artist::mock::opengl::glFunctionMock::instance()->glGetActiveUniform_mock\n\n  Line 65: #define glGetAttachedShaders artist::mock::opengl::glFunctionMock::instance()->glGetAttachedShaders_mock\n\n  Line 66: #define glGetAttribLocation artist::mock::opengl::glFunctionMock::instance()->glGetAttribLocation_mock\n\n  Line 67: #define glGetProgramInfoLog artist::mock::opengl::glFunctionMock::instance()->glGetProgramInfoLog_mock\n\n  Line 68: #define glGetProgramiv artist::mock::opengl::glFunctionMock::instance()->glGetProgramiv_mock\n\n  Line 69: #define glGetShaderInfoLog artist::mock::opengl::glFunctionMock::instance()->glGetShaderInfoLog_mock\n\n  Line 70: #define glGetShaderSource artist::mock::opengl::glFunctionMock::instance()->glGetShaderSource_mock\n\n  Line 71: #define glGetShaderiv artist::mock::opengl::glFunctionMock::instance()->glGetShaderiv_mock\n\n  Line 72: #define glGetUniformLocation artist::mock::opengl::glFunctionMock::instance()->glGetUniformLocation_mock\n\n  Line 73: #define glGetUniformfv artist::mock::opengl::glFunctionMock::instance()->glGetUniformfv_mock\n\n  Line 74: #define glGetUniformiv artist::mock::opengl::glFunctionMock::instance()->glGetUniformiv_mock\n\n  Line 75: #define glGetVertexAttribPointerv artist::mock::opengl::glFunctionMock::instance()->glGetVertexAttribPointerv_mock\n\n  Line 76: #define glGetVertexAttribdv artist::mock::opengl::glFunctionMock::instance()->glGetVertexAttribdv_mock\n\n\n(Results are truncated: showing first 40 results. Consider using a more specific path or pattern.)"
    }
  ],
  "metadata": {
    "matches": 40,
    "truncated": true
  }
}
```

**outil `glob`** :

```json
{
  "status": "completed",
  "input": {
    "pattern": "ARTIST/tests/src/graphic/opengl/pipeline/component/**/*.cpp",
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST"
  },
  "content": [
    {
      "type": "text",
      "text": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\ReaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\LoaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\FreerTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\UnbinderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\SetterTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\BinderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pipeline\\UserTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pipeline\\ResetterTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\UserTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\LoaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\FreerTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\UniformReaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\AttributeReaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\ShaderAttacherTests.cpp"
    }
  ],
  "metadata": {
    "count": 14,
    "truncated": false
  }
}
```

**outil `glob`** :

```json
{
  "status": "completed",
  "input": {
    "pattern": "ARTIST/include/graphic/**/{Attribute,Uniform}*.hpp",
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST"
  },
  "content": [
    {
      "type": "text",
      "text": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\pipeline\\Uniform.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\pipeline\\Attribute.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\context\\UniformContext.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\context\\AttributeContext.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\validator\\UniformConcept.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\validator\\AttributeConcept.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\profile\\Attribute.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\profile\\Uniform.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\context\\UniformContext.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\context\\AttributeContext.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\UniformReader.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\AttributeReader.hpp"
    }
  ],
  "metadata": {
    "count": 12,
    "truncated": false
  }
}
```

**outil `grep`** :

```json
{
  "status": "completed",
  "input": {
    "pattern": "glDeleteBuffers|glIsBuffer",
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include",
    "limit": 20
  },
  "content": [
    {
      "type": "text",
      "text": "No matches found"
    }
  ],
  "metadata": {
    "matches": 0,
    "truncated": false
  }
}
```

### [39] assistant @ 2026-10-19 07:36:15.502Z — opencode-go/glm-5.3-flash/max

**reasoning** :

Key findings:
1. glFunctionMock.hpp is the mock file itself. I need to find which files INCLUDE it — let me grep for `#include.*glFunctionMock` in tests and include.
2. Tests confirmed: attribute/{Unbinder,Setter,Binder}Tests.cpp, pass/{Loader,UniformReader,AttributeReader,Freer,User,ShaderAttacher}Tests.cpp, shader/{Reader,Loader,Freer}Tests.cpp. Note: pass/LoaderTests.cpp exists but there's no pass/Loader.hpp production file visible? Let me check. Also note the doc's H branch says "les deux LoaderTests" — pass/LoaderTests.cpp and shader/LoaderTests.cpp. Confirmed both exist.
3. Attribute files: pipeline/Attribute.hpp, context/AttributeContext.hpp, opengl/context/AttributeContext.hpp, opengl/profile/Attribute.hpp, validator/AttributeConcept.hpp — these are additional files the doc's branch A mentions as "contexte Attribute, composants attribute/". Good.
4. No glDeleteBuffers/glIsBuffer in ARTIST/include — confirms the observation.

Now let me check:
- Who includes glFunctionMock.hpp (test files + any include files).
- Whether AttributeReader.hpp/UniformReader.hpp include opengl/context/UniformContext.hpp (B/C overlap).
- Whether Setter.hpp (uniform component) includes UniformContext.
- Whether pass/Loader.hpp production file exists (H branch says "Pas de Loader production" — but pass/LoaderTests.cpp tests what?).
- The 12 formes/Pointer/IPointer tests — where do Attribute tests live? Maybe tests/src/graphic/opengl/context/ or similar. Let me glob tests dirs for Attribute.

Constats initiaux : `glDeleteBuffers`/`glIsBuffer` absents de `ARTIST/include` (observation confirmée), et des tests Readers `pass/AttributeReaderTests.cpp`/`UniformReaderTests.cpp` existent. Je creuse les inclusions et recouvrements.

**outil `grep`** :

```json
{
  "status": "completed",
  "input": {
    "limit": 40,
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST",
    "pattern": "#include.*glFunctionMock"
  },
  "content": [
    {
      "type": "text",
      "text": "Found 20 matches\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\AttributeOwnershipTests.cpp:\n  Line 8: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\LoaderTests.cpp:\n  Line 13: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\PassTests.cpp:\n  Line 6: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\UnbinderTests.cpp:\n  Line 6: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\FreerTests.cpp:\n  Line 6: #include <opengl/glFunctionMock.hpp>\r\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\BinderTests.cpp:\n  Line 6: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\SetterTests.cpp:\n  Line 7: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\ReaderTests.cpp:\n  Line 18: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\AttributeReaderTests.cpp:\n  Line 9: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\ShaderTests.cpp:\n  Line 6: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\ShaderLifetimeTests.cpp:\n  Line 11: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformCardinalityTests.cpp:\n  Line 7: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformOwnershipTests.cpp:\n  Line 9: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\LoaderTests.cpp:\n  Line 13: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformTests.cpp:\n  Line 7: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\FreerTests.cpp:\n  Line 6: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\ShaderAttacherTests.cpp:\n  Line 7: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformValueTests.cpp:\n  Line 8: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\UniformReaderTests.cpp:\n  Line 10: #include <opengl/glFunctionMock.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\UserTests.cpp:\n  Line 6: #include <opengl/glFunctionMock.hpp>\n"
    }
  ],
  "metadata": {
    "matches": 20,
    "truncated": false
  }
}
```

**outil `grep`** :

```json
{
  "status": "completed",
  "input": {
    "limit": 30,
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass",
    "pattern": "#include.*(UniformContext|AttributeContext|PassContext)"
  },
  "content": [
    {
      "type": "text",
      "text": "Found 7 matches\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\User.hpp:\n  Line 7: #include <graphic/opengl/context/PassContext.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\UniformReader.hpp:\n  Line 9: #include <graphic/opengl/context/PassContext.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\Freer.hpp:\n  Line 7: #include <graphic/opengl/context/PassContext.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\Loader.hpp:\n  Line 11: #include <graphic/opengl/context/PassContext.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\ShaderAttacher.hpp:\n  Line 6: #include <graphic/opengl/context/PassContext.hpp>\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\AttributeReader.hpp:\n  Line 9: #include <graphic/opengl/context/PassContext.hpp>\n\n  Line 14: #include <graphic/opengl/context/AttributeContext.hpp>\n"
    }
  ],
  "metadata": {
    "matches": 7,
    "truncated": false
  }
}
```

**outil `glob`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST",
    "pattern": "ARTIST/include/graphic/opengl/pipeline/component/pass/*.hpp"
  },
  "content": [
    {
      "type": "text",
      "text": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\Loader.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\Freer.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\AttributeReader.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\ShaderAttacher.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\User.hpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\pass\\UniformReader.hpp"
    }
  ],
  "metadata": {
    "count": 6,
    "truncated": false
  }
}
```

**outil `glob`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST",
    "pattern": "ARTIST/tests/src/graphic/**/*.{cpp,hpp}"
  },
  "content": [
    {
      "type": "text",
      "text": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformValueTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformReaderFlowContractTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformOwnershipTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformContractTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformCardinalityTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\ShaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\ShaderLifetimeTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\PipelineTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\PipelineOwnershipTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\PipelineCursorTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\PipelineBorrowedContractTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\PassTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\AttributeTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\AttributeOwnershipTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\AttributeContractTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\context\\ShaderContextTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\context\\PipelineContextTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\context\\PassContextTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\context\\AttributeContextTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\ReaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\LoaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\FreerTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\UserTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\UniformReaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\ShaderAttacherTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\LoaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\FreerTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\AttributeReaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\UnbinderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pipeline\\UserTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pipeline\\ResetterTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\BinderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\attribute\\SetterTests.cpp"
    }
  ],
  "metadata": {
    "count": 34,
    "truncated": false
  }
}
```

### [62] assistant @ 2026-10-19 07:36:24.112Z — opencode-go/glm-5.3-flash/max

**reasoning** :

Very informative. Key findings:

1. **glFunctionMock.hpp consumers** (20 test files): notably `tests/src/graphic/pipeline/AttributeOwnershipTests.cpp`, `UniformOwnershipTests.cpp`, `UniformCardinalityTests.cpp`, `UniformValueTests.cpp`, `UniformTests.cpp`, `AttributeTests.cpp`, `AttributeContractTests.cpp`, `PassTests.cpp`, plus GL component tests (BinderTests, SetterTests, UnbinderTests, AttributeReaderTests, UniformReaderTests, LoaderTests ×2, FreerTests ×2, ShaderAttacherTests, UserTests ×2, ReaderTests, ShaderTests, ShaderLifetimeTests). So M's ownership of glFunctionMock.hpp touches files owned by A (attribute tests) and C (uniform tests) and B (readers tests) — confirming why M must be integrated BEFORE authors, not in parallel. This validates the doc's constraint "Intégrer M avant ses consommateurs".

2. **UniformReader.hpp does NOT include UniformContext.hpp** — it includes only PassContext.hpp (GL). AttributeReader.hpp includes PassContext.hpp + AttributeContext.hpp. So no direct B→C header overlap via UniformContext. But B owns the Readers, and C owns UniformContext (both base + GL) and Setter. Overlap risk: UniformReader publishes via openglContext.addUniform → base PassContext::addUniform (owned by B per doc: B owns BOTH PassContext files). But wait — base PassContext::addUniform is the replacement-owner logic which interacts with Uniform storage. If C needs to change context/UniformContext.hpp (addUniform/addAttribute might live in PassContext, not UniformContext)... Let me verify where addUniform/addAttribute live. The user said: base PassContext.hpp has addUniform L51, addAttribute L67. And UniformContext exists separately. So the "contexte Uniform" ownership of C = UniformContext.hpp base + GL. But the publication path for Uniform goes through PassContext::addUniform (owned by B). So K decision #4 (interactions: responsibility of PassContext during replacement; stable metadata interfaces between B/C) is well-founded.

Let me verify: read base PassContext.hpp quickly, UniformContext base, and check who calls addUniform/addAttribute. Also check Setter.hpp (uniform component) — which context it uses. And check whether UniformReader includes UniformContext indirectly (via PassContext? probably PassContext includes UniformContext).

3. **Tests overlap detection**: 
   - Branch A owns: attribute/{Binder,Setter,Unbinder}Tests.cpp (GL), pipeline/AttributeTests.cpp, AttributeOwnershipTests.cpp, AttributeContractTests.cpp, context/AttributeContextTests.cpp.
   - Branch B owns: pass/{AttributeReader,UniformReader}Tests.cpp, context/PassContextTests.cpp, pipeline/UniformReaderFlowContractTests.cpp? Hmm — UniformReaderFlowContractTests.cpp is a pipeline-level test about UniformReader flow. Which branch? It involves UniformReader (B) but also Uniform (C). Let me check its content briefly.
   - Branch C owns: pipeline/Uniform{Tests,OwnershipTests,CardinalityTests,ValueTests,ContractTests}.cpp, uniform/Setter tests? There's no uniform/SetterTests.cpp in the GL component listing — attribute has SetterTests but uniform doesn't (no uniform/ dir in GL component tests listed: only attribute/ and pipeline/ and pass/ and shader/). Let me check if there's a uniform component dir in include: ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp exists per user. Tests for Setter uniform may live in UniformValueTests or pass-level.

4. **pass/Loader.hpp exists** — production file. H branch says "Pas de Loader production" — fine, H only touches the two LoaderTests.cpp files. But note: pass/LoaderTests.cpp + shader/LoaderTests.cpp both include glFunctionMock.hpp (mock singleton) — H's ownership overlaps M's file only via include, not modification. That's fine — including a mock doesn't require owning it. But if M extends glFunctionMock.hpp, H's tests could be affected. Another reason M integrates before authors.

5. The doc's claim "B owns context/PassContext.hpp" — but User.hpp, Freer.hpp, Loader.hpp, ShaderAttacher.hpp also include PassContext.hpp (GL). B modifying base+GL PassContext could break those, but that's fine — those are consumers, not owners. However: does branch A's attribute publication also go through PassContext::addAttribute? AttributeReader publishes via openglContext.addAttribute — B owns PassContext (both files). But A owns "contexte Attribute" = AttributeContext base+GL. And K#4 says PassContext responsibility during replacement of an Attribute that's now VBO owner. So the addAttribute owner-replacement logic lives in base PassContext — owned by B, touched by A's contract. That's the K#4 interaction. Confirmed by user's statement.

Let me now verify:
- Base PassContext.hpp content (addUniform L51, addAttribute L67) and whether it includes UniformContext/AttributeContext.
- UniformContext base/GL content briefly.
- Setter.hpp (uniform) — what it includes.
- UniformReaderFlowContractTests.cpp — what it tests, to assign ownership.
- Check CMake test registration for LoaderTests (H) — does CMake need changes? H says no CMake. Check tests/CMakeLists.txt for the two LoaderTests files and whether helper files are globbed or listed.

Let me batch these reads/greps.

Points clés : 20 fichiers de tests incluent le mock singleton `glFunctionMock.hpp` (A, B, C, H tous consommateurs → valide la barrière M avant auteurs), et `UniformReader.hpp` n'inclut pas directement `UniformContext.hpp` (seulement `PassContext.hpp`). Je vérifie les dernières intersections : contenu de `PassContext` (base), `Setter.hpp` uniform, tests `UniformReaderFlowContractTests` et enregistrement CMake des LoaderTests.

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\context\\PassContext.hpp"
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\context\\PassContext.hpp, lines 1-167\n1: // PassContext.hpp\n2: \n3: #pragma once\n4: #include <vector>\n5: #include <unordered_map>\n6: #include <memory>\n7: #include <format>\n8: #include <utils.hpp>\n9: #include <graphic/Api.hpp>\n10: #include <graphic/pipeline/Shader.hpp>\n11: #include <graphic/pipeline/Uniform.hpp>\n12: #include <graphic/pipeline/Attribute.hpp>\n13: \n14: namespace artist::graphic::context\n15: {\n16:     namespace pipeline = artist::graphic::pipeline;\n17:     /**\n18:      * @class PassContext\n19:      * @brief Abstract base class for pass context.\n20:      *\n21:      * The PassContext class provides an interface for managing the details and state of a shader pass.\n22:      * This includes a collection of shaders and uniform variables. It serves as a base for API-specific\n23:      * implementations tailored to different graphics APIs.\n24:      */\n25:     template <typename API>\n26:     class PassContext\n27:     {\n28:     public:\n29:         virtual ~PassContext() = default;\n30: \n31:         /**\n32:          * @brief Take unique ownership of a shader in the pass.\n33:          * @param shader Shader to own.\n34:          */\n35:         void addShader(std::unique_ptr<pipeline::IShader<API>> shader)\n36:         {\n37:             if (!shader)\n38:             {\n39:                 throw common::exception::TraceableException<std::invalid_argument>(\"ERROR::PASS::NULL_SHADER\");\n40:             }\n41:             m_shaders.push_back(std::move(shader));\n42:         }\n43: \n44:         /**\n45:          * @brief Take unique ownership of a uniform in the pass.\n46:          *\n47:          * @param name Name of the uniform; a uniform already registered under it is replaced.\n48:          * @param uniform Uniform to own.\n49:          * @throws ERROR::PASS::NULL_UNIFORM if `uniform` is null; the uniforms are left unchanged.\n50:          */\n51:         void addUniform(const std::string &name, std::unique_ptr<pipeline::Uniform<API>> uniform)\n52:         {\n53:             if (!uniform)\n54:             {\n55:                 throw common::exception::TraceableException<std::invalid_argument>(\"ERROR::PASS::NULL_UNIFORM\");\n56:             }\n57:             m_uniforms[name] = std::move(uniform);\n58:         }\n59: \n60:         /**\n61:          * @brief Take unique ownership of an attribute in the pass.\n62:          *\n63:          * @param name Name of the attribute; an attribute already registered under it is replaced.\n64:          * @param attribute Attribute to own.\n65:          * @throws ERROR::PASS::NULL_ATTRIBUTE if `attribute` is null; the attributes are left unchanged.\n66:          */\n67:         void addAttribute(const std::string &name, std::unique_ptr<pipeline::IAttribute<API>> attribute)\n68:         {\n69:             if (!attribute)\n70:             {\n71:                 throw common::exception::TraceableException<std::invalid_argument>(\"ERROR::PASS::NULL_ATTRIBUTE\");\n72:             }\n73:             m_attributes[name] = std::move(attribute);\n74:         }\n75: \n76:         /**\n77:          * @brief Borrow the shader at the given index.\n78:          * @param index Index of the shader to get.\n79:          * @return Borrowed shader at the given index.\n80:          *\n81:          * F4 (mission C): out-of-range and negative accesses are rejected\n82:          * with the business exception ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\n83:          * before any indexing; the negative check happens before the\n84:          * size_type conversion so -1 cannot wrap into a huge offset, and the\n85:          * range comparison itself is done in vector::size_type. vector::at\n86:          * is intentionally not used. In-range borrows keep returning the\n87:          * registered object; a rejection leaves the context untouched.\n88:          */\n89:         [[nodiscard]] pipeline::IShader<API> &getShader(const int &index) const\n90:         {\n91:             if (index < 0 || static_cast<decltype(m_shaders)::size_type>(index) >= m_shaders.size())\n92:             {\n93:                 throw common::exception::TraceableException<std::out_of_range>(std::format(\"ERROR::PASS::SHADER_INDEX_OUT_OF_RANGE\\nShader index {} out of range (size {})\", index, m_shaders.size()));\n94:             }\n95:             return *m_shaders[static_cast<decltype(m_shaders)::size_type>(index)];\n96:         }\n97: \n98:         /**\n99:          * @brief Borrow the uniform with the given name.\n100:          * @param name Name of the uniform to get.\n101:          * @return Borrowed uniform owned by the pass, or nullptr if no uniform has this name.\n102:          */\n103:         [[nodiscard]] pipeline::Uniform<API> *getUniform(const std::string &name) const\n104:         {\n105:             if (m_uniforms.contains(name))\n106:             {\n107:                 return m_uniforms.at(name).get();\n108:             }\n109:             return nullptr;\n110:         }\n111: \n112:         /**\n113:          * @brief Borrow the attribute with the given name.\n114:          * @param name Name of the attribute to get.\n115:          * @return Borrowed attribute owned by the pass, or nullptr if no attribute has this name.\n116:          */\n117:         [[nodiscard]] pipeline::IAttribute<API> *getAttribute(const std::string &name) const\n118:         {\n119:             if (m_attributes.contains(name))\n120:             {\n121:                 return m_attributes.at(name).get();\n122:             }\n123:             return nullptr;\n124:         }\n125: \n126:         /**\n127:          * @brief Get the number of shaders in the pass.\n128:          * @return Number of shaders in the pass.\n129:          */\n130:         [[nodiscard]] int getShadersCount() const\n131:         {\n132:             return m_shaders.size();\n133:         }\n134: \n135:         /**\n136:          * @brief Borrow the shaders in the pass.\n137:          * @return Shaders in the pass, in registration order.\n138:          */\n139:         [[nodiscard]] const std::vector<std::unique_ptr<pipeline::IShader<API>>> &getShaders() const\n140:         {\n141:             return m_shaders;\n142:         }\n143: \n144:         /**\n145:          * @brief Borrow the uniforms owned by the pass.\n146:          * @return Uniforms in the pass, none of them null.\n147:          */\n148:         [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<pipeline::Uniform<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getUniforms() const\n149:         {\n150:             return m_uniforms;\n151:         }\n152: \n153:         /**\n154:          * @brief Borrow the attributes owned by the pass.\n155:          * @return Attributes in the pass, none of them null.\n156:          */\n157:         [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<pipeline::IAttribute<API>>, collection_utils::StringHash, collection_utils::StringEqual> &getAttributes() const\n158:         {\n159:             return m_attributes;\n160:         }\n161: \n162:     private:\n163:         std::vector<std::unique_ptr<pipeline::IShader<API>>> m_shaders;\n164:         std::unordered_map<std::string, std::unique_ptr<pipeline::Uniform<API>>, collection_utils::StringHash, collection_utils::StringEqual> m_uniforms;\n165:         std::unordered_map<std::string, std::unique_ptr<pipeline::IAttribute<API>>, collection_utils::StringHash, collection_utils::StringEqual> m_attributes;\n166:     };\n167: }"
    }
  ],
  "metadata": {
    "truncated": false
  }
}
```

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\uniform\\Setter.hpp"
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\uniform\\Setter.hpp, lines 1-504\n1: // file: Setter\n2: \n3: #pragma once\n4: \n5: #include <GL/glew.h>\n6: #include <format>\n7: #include <stdexcept>\n8: #include <string>\n9: #include <memory>\n10: #include <glm/glm.hpp>\n11: #include <glm/gtc/type_ptr.hpp>\n12: #include <graphic/opengl/context/UniformContext.hpp>\n13: \n14: #include <graphic/Api.hpp>\n15: #include <common/exception/TraceableException.hpp>\n16: \n17: namespace artist::graphic::opengl::pipeline::component::uniform\n18: {\n19:     namespace detail\n20:     {\n21:         inline void requireSingleValue(const artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n22:         {\n23:             // Uniform::set stores one T, not an array of reflected GL elements.\n24:             // Reject before GL, including scalar setters whose API has no count.\n25:             if (uniform.getGLSize() != 1)\n26:             {\n27:                 throw artist::common::exception::TraceableException<std::runtime_error>(\n28:                     std::format(\"ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED size={}\", uniform.getGLSize()));\n29:             }\n30:         }\n31:     }\n32: \n33:     template <typename U>\n34:     struct OpenGLUniformSetter\n35:     {\n36:         static constexpr GLenum glType = GL_INVALID_ENUM;\n37:     };\n38:     /**\n39:      * @brief Specialization for GLfloat with corresponding setter\n40:      */\n41:     template <>\n42:     struct OpenGLUniformSetter<GLfloat>\n43:     {\n44:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n45:         {\n46:             detail::requireSingleValue(uniform);\n47:             glUniform1f(uniform.getUniformID(), uniform.getValue<GLfloat>());\n48:         }\n49: \n50:         static constexpr GLenum glType = GL_FLOAT;\n51:     };\n52: \n53:     /**\n54:      * @brief Specialization for GLint with corresponding setter\n55:      */\n56:     template <>\n57:     struct OpenGLUniformSetter<GLint>\n58:     {\n59:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n60:         {\n61:             detail::requireSingleValue(uniform);\n62:             glUniform1i(uniform.getUniformID(), uniform.getValue<GLint>());\n63:         }\n64: \n65:         static constexpr GLenum glType = GL_INT;\n66:     };\n67: \n68:     /**\n69:      * @brief Specialization for GLuint with corresponding setter\n70:      */\n71:     template <>\n72:     struct OpenGLUniformSetter<GLuint>\n73:     {\n74:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n75:         {\n76:             detail::requireSingleValue(uniform);\n77:             glUniform1ui(uniform.getUniformID(), uniform.getValue<GLuint>());\n78:         }\n79: \n80:         static constexpr GLenum glType = GL_UNSIGNED_INT;\n81:     };\n82: \n83:     /**\n84:      * @brief Specialization for GLdouble with corresponding setter\n85:      */\n86:     template <>\n87:     struct OpenGLUniformSetter<GLdouble>\n88:     {\n89:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n90:         {\n91:             detail::requireSingleValue(uniform);\n92:             glUniform1d(uniform.getUniformID(), uniform.getValue<GLdouble>());\n93:         }\n94: \n95:         static constexpr GLenum glType = GL_DOUBLE;\n96:     };\n97: \n98:     /**\n99:      * @brief Specialization for glm::vec2 with corresponding setter\n100:      */\n101:     template <>\n102:     struct OpenGLUniformSetter<glm::vec2>\n103:     {\n104:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n105:         {\n106:             detail::requireSingleValue(uniform);\n107:             glUniform2fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec2>()[0]);\n108:         }\n109: \n110:         static constexpr GLenum glType = GL_FLOAT_VEC2;\n111:     };\n112: \n113:     /**\n114:      * @brief Specialization for glm::vec3 with corresponding setter\n115:      */\n116:     template <>\n117:     struct OpenGLUniformSetter<glm::vec3>\n118:     {\n119:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n120:         {\n121:             detail::requireSingleValue(uniform);\n122:             glUniform3fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec3>()[0]);\n123:         }\n124: \n125:         static constexpr GLenum glType = GL_FLOAT_VEC3;\n126:     };\n127: \n128:     /**\n129:      * @brief Specialization for glm::vec4 with corresponding setter\n130:      */\n131:     template <>\n132:     struct OpenGLUniformSetter<glm::vec4>\n133:     {\n134:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n135:         {\n136:             detail::requireSingleValue(uniform);\n137:             glUniform4fv(uniform.getUniformID(), 1, &uniform.getValue<glm::vec4>()[0]);\n138:         }\n139: \n140:         static constexpr GLenum glType = GL_FLOAT_VEC4;\n141:     };\n142: \n143:     /**\n144:      * @brief Specialization for glm::dvec2 with corresponding setter\n145:      */\n146:     template <>\n147:     struct OpenGLUniformSetter<glm::dvec2>\n148:     {\n149:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n150:         {\n151:             detail::requireSingleValue(uniform);\n152:             glUniform2dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec2>()[0]);\n153:         };\n154: \n155:         static constexpr GLenum glType = GL_DOUBLE_VEC2;\n156:     };\n157: \n158:     /**\n159:      * @brief Specialization for glm::dvec3 with corresponding setter\n160:      */\n161:     template <>\n162:     struct OpenGLUniformSetter<glm::dvec3>\n163:     {\n164:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n165:         {\n166:             detail::requireSingleValue(uniform);\n167:             glUniform3dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec3>()[0]);\n168:         };\n169: \n170:         static constexpr GLenum glType = GL_DOUBLE_VEC3;\n171:     };\n172: \n173:     /**\n174:      * @brief Specialization for glm::dvec4 with corresponding setter\n175:      */\n176:     template <>\n177:     struct OpenGLUniformSetter<glm::dvec4>\n178:     {\n179:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n180:         {\n181:             detail::requireSingleValue(uniform);\n182:             glUniform4dv(uniform.getUniformID(), 1, &uniform.getValue<glm::dvec4>()[0]);\n183:         };\n184: \n185:         static constexpr GLenum glType = GL_DOUBLE_VEC4;\n186:     };\n187: \n188:     // Specialization for glm::ivec2\n189:     template <>\n190:     struct OpenGLUniformSetter<glm::ivec2>\n191:     {\n192:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n193:         {\n194:             detail::requireSingleValue(uniform);\n195:             glUniform2iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec2>()));\n196:         }\n197: \n198:         static constexpr GLenum glType = GL_INT_VEC2;\n199:     };\n200: \n201:     // Specialization for glm::ivec3\n202:     template <>\n203:     struct OpenGLUniformSetter<glm::ivec3>\n204:     {\n205:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n206:         {\n207:             detail::requireSingleValue(uniform);\n208:             glUniform3iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec3>()));\n209:         }\n210: \n211:         static constexpr GLenum glType = GL_INT_VEC3;\n212:     };\n213: \n214:     // Specialization for glm::ivec4\n215:     template <>\n216:     struct OpenGLUniformSetter<glm::ivec4>\n217:     {\n218:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n219:         {\n220:             detail::requireSingleValue(uniform);\n221:             glUniform4iv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::ivec4>()));\n222:         }\n223: \n224:         static constexpr GLenum glType = GL_INT_VEC4;\n225:     };\n226: \n227:     // Specialization for glm::uvec2 (unsigned int vector)\n228:     template <>\n229:     struct OpenGLUniformSetter<glm::uvec2>\n230:     {\n231:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n232:         {\n233:             detail::requireSingleValue(uniform);\n234:             glUniform2uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec2>()));\n235:         }\n236: \n237:         static constexpr GLenum glType = GL_UNSIGNED_INT_VEC2;\n238:     };\n239: \n240:     // Specialization for glm::uvec3 (unsigned int vector)\n241:     template <>\n242:     struct OpenGLUniformSetter<glm::uvec3>\n243:     {\n244:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n245:         {\n246:             detail::requireSingleValue(uniform);\n247:             glUniform3uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec3>()));\n248:         }\n249: \n250:         static constexpr GLenum glType = GL_UNSIGNED_INT_VEC3;\n251:     };\n252: \n253:     // Specialization for glm::uvec4 (unsigned int vector)\n254:     template <>\n255:     struct OpenGLUniformSetter<glm::uvec4>\n256:     {\n257:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n258:         {\n259:             detail::requireSingleValue(uniform);\n260:             glUniform4uiv(uniform.getUniformID(), 1, glm::value_ptr(uniform.getValue<glm::uvec4>()));\n261:         }\n262: \n263:         static constexpr GLenum glType = GL_UNSIGNED_INT_VEC4;\n264:     };\n265: \n266:     // Specialization for glm::mat2\n267:     template <>\n268:     struct OpenGLUniformSetter<glm::mat2>\n269:     {\n270:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n271:         {\n272:             detail::requireSingleValue(uniform);\n273:             glUniformMatrix2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2>()));\n274:         }\n275: \n276:         static constexpr GLenum glType = GL_FLOAT_MAT2;\n277:     };\n278: \n279:     // Specialization for glm::mat3\n280:     template <>\n281:     struct OpenGLUniformSetter<glm::mat3>\n282:     {\n283:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n284:         {\n285:             detail::requireSingleValue(uniform);\n286:             glUniformMatrix3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3>()));\n287:         }\n288: \n289:         static constexpr GLenum glType = GL_FLOAT_MAT3;\n290:     };\n291: \n292:     // Specialization for glm::mat4\n293:     template <>\n294:     struct OpenGLUniformSetter<glm::mat4>\n295:     {\n296:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n297:         {\n298:             detail::requireSingleValue(uniform);\n299:             glUniformMatrix4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4>()));\n300:         }\n301: \n302:         static constexpr GLenum glType = GL_FLOAT_MAT4;\n303:     };\n304: \n305:     // Specialization for glm::mat2x3\n306:     template <>\n307:     struct OpenGLUniformSetter<glm::mat2x3>\n308:     {\n309:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n310:         {\n311:             detail::requireSingleValue(uniform);\n312:             glUniformMatrix2x3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2x3>()));\n313:         }\n314: \n315:         static constexpr GLenum glType = GL_FLOAT_MAT2x3;\n316:     };\n317: \n318:     // Specialization for glm::mat3x2\n319:     template <>\n320:     struct OpenGLUniformSetter<glm::mat3x2>\n321:     {\n322:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n323:         {\n324:             detail::requireSingleValue(uniform);\n325:             glUniformMatrix3x2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3x2>()));\n326:         }\n327: \n328:         static constexpr GLenum glType = GL_FLOAT_MAT3x2;\n329:     };\n330: \n331:     // Specialization for glm::mat2x4\n332:     template <>\n333:     struct OpenGLUniformSetter<glm::mat2x4>\n334:     {\n335:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n336:         {\n337:             detail::requireSingleValue(uniform);\n338:             glUniformMatrix2x4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat2x4>()));\n339:         }\n340: \n341:         static constexpr GLenum glType = GL_FLOAT_MAT2x4;\n342:     };\n343: \n344:     // Specialization for glm::mat4x2\n345:     template <>\n346:     struct OpenGLUniformSetter<glm::mat4x2>\n347:     {\n348:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n349:         {\n350:             detail::requireSingleValue(uniform);\n351:             glUniformMatrix4x2fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4x2>()));\n352:         }\n353: \n354:         static constexpr GLenum glType = GL_FLOAT_MAT4x2;\n355:     };\n356: \n357:     // Specialization for glm::mat3x4\n358:     template <>\n359:     struct OpenGLUniformSetter<glm::mat3x4>\n360:     {\n361:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n362:         {\n363:             detail::requireSingleValue(uniform);\n364:             glUniformMatrix3x4fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat3x4>()));\n365:         }\n366: \n367:         static constexpr GLenum glType = GL_FLOAT_MAT3x4;\n368:     };\n369: \n370:     // Specialization for glm::mat4x3\n371:     template <>\n372:     struct OpenGLUniformSetter<glm::mat4x3>\n373:     {\n374:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n375:         {\n376:             detail::requireSingleValue(uniform);\n377:             glUniformMatrix4x3fv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::mat4x3>()));\n378:         }\n379: \n380:         static constexpr GLenum glType = GL_FLOAT_MAT4x3;\n381:     };\n382: \n383:     /**\n384:      * @brief Specialization for glm::mat4 with corresponding setter\n385:      */\n386: \n387:     // Specialization for glm::dmat2\n388:     template <>\n389:     struct OpenGLUniformSetter<glm::dmat2>\n390:     {\n391:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n392:         {\n393:             detail::requireSingleValue(uniform);\n394:             glUniformMatrix2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2>()));\n395:         }\n396: \n397:         static constexpr GLenum glType = GL_DOUBLE_MAT2;\n398:     };\n399: \n400:     // Specialization for glm::dmat3\n401:     template <>\n402:     struct OpenGLUniformSetter<glm::dmat3>\n403:     {\n404:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n405:         {\n406:             detail::requireSingleValue(uniform);\n407:             glUniformMatrix3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3>()));\n408:         }\n409: \n410:         static constexpr GLenum glType = GL_DOUBLE_MAT3;\n411:     };\n412: \n413:     // Specialization for glm::dmat4\n414:     template <>\n415:     struct OpenGLUniformSetter<glm::dmat4>\n416:     {\n417:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n418:         {\n419:             detail::requireSingleValue(uniform);\n420:             glUniformMatrix4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4>()));\n421:         }\n422: \n423:         static constexpr GLenum glType = GL_DOUBLE_MAT4;\n424:     };\n425: \n426:     // Specialization for glm::dmat2x3\n427:     template <>\n428:     struct OpenGLUniformSetter<glm::dmat2x3>\n429:     {\n430:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n431:         {\n432:             detail::requireSingleValue(uniform);\n433:             glUniformMatrix2x3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2x3>()));\n434:         }\n435: \n436:         static constexpr GLenum glType = GL_DOUBLE_MAT2x3;\n437:     };\n438: \n439:     // Specialization for glm::dmat3x2\n440:     template <>\n441:     struct OpenGLUniformSetter<glm::dmat3x2>\n442:     {\n443:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n444:         {\n445:             detail::requireSingleValue(uniform);\n446:             glUniformMatrix3x2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3x2>()));\n447:         }\n448: \n449:         static constexpr GLenum glType = GL_DOUBLE_MAT3x2;\n450:     };\n451: \n452:     // Specialization for glm::dmat2x4\n453:     template <>\n454:     struct OpenGLUniformSetter<glm::dmat2x4>\n455:     {\n456:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n457:         {\n458:             detail::requireSingleValue(uniform);\n459:             glUniformMatrix2x4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat2x4>()));\n460:         }\n461: \n462:         static constexpr GLenum glType = GL_DOUBLE_MAT2x4;\n463:     };\n464: \n465:     // Specialization for glm::dmat4x2\n466:     template <>\n467:     struct OpenGLUniformSetter<glm::dmat4x2>\n468:     {\n469:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n470:         {\n471:             detail::requireSingleValue(uniform);\n472:             glUniformMatrix4x2dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4x2>()));\n473:         }\n474: \n475:         static constexpr GLenum glType = GL_DOUBLE_MAT4x2;\n476:     };\n477: \n478:     // Specialization for glm::dmat3x4\n479:     template <>\n480:     struct OpenGLUniformSetter<glm::dmat3x4>\n481:     {\n482:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n483:         {\n484:             detail::requireSingleValue(uniform);\n485:             glUniformMatrix3x4dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat3x4>()));\n486:         }\n487: \n488:         static constexpr GLenum glType = GL_DOUBLE_MAT3x4;\n489:     };\n490: \n491:     // Specialization for glm::dmat4x3\n492:     template <>\n493:     struct OpenGLUniformSetter<glm::dmat4x3>\n494:     {\n495:         static void on(artist::graphic::opengl::context::OpenGLUniformContext &uniform)\n496:         {\n497:             detail::requireSingleValue(uniform);\n498:             glUniformMatrix4x3dv(uniform.getUniformID(), 1, GL_FALSE, glm::value_ptr(uniform.getValue<glm::dmat4x3>()));\n499:         }\n500: \n501:         static constexpr GLenum glType = GL_DOUBLE_MAT4x3;\n502:     };\n503: \n504: }"
    }
  ],
  "metadata": {
    "truncated": false
  }
}
```

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformReaderFlowContractTests.cpp",
    "limit": 60
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\pipeline\\UniformReaderFlowContractTests.cpp, lines 1-52\n1: #include <gtest/gtest.h>\n2: #include <memory>\n3: \n4: #include <graphic/opengl/profile/Pass.hpp>\n5: #include <graphic/opengl/validator/PassConcept.hpp>\n6: \n7: namespace\n8: {\n9:     struct BorrowedUniformFlowComponent\n10:     {\n11:         template <typename C> static void on(C &) {}\n12:         template <typename C> static void on(C &, GLuint) {}\n13:     };\n14:     struct AbsentUniformReader {};\n15:     struct SharedOnlyUniformReader\n16:     {\n17:         template <typename C> static void on(std::shared_ptr<C>) {}\n18:     };\n19:     struct NonVoidUniformReader\n20:     {\n21:         template <typename C> static int on(C &) { return 0; }\n22:     };\n23:     template <typename R>\n24:     struct UniformReaderContractAPI\n25:     {\n26:         struct PassContext\n27:         {\n28:             template <auto> using Loader = BorrowedUniformFlowComponent;\n29:             template <auto> using Freer = BorrowedUniformFlowComponent;\n30:             template <auto> using ShaderAttacher = BorrowedUniformFlowComponent;\n31:             template <auto> using AttributeReader = BorrowedUniformFlowComponent;\n32:             template <auto> using User = BorrowedUniformFlowComponent;\n33:             template <auto> using UniformReader = R;\n34:         };\n35:     };\n36:     constexpr auto ClassicUniformFlow = artist::graphic::opengl::profile::Pass::Classic;\n37:     template <typename R>\n38:     concept ValidUniformReaderFlow = artist::graphic::opengl::validator::OpenGLPassFlow<UniformReaderContractAPI<R>, ClassicUniformFlow>;\n39: \n40:     static_assert(ValidUniformReaderFlow<BorrowedUniformFlowComponent>);\n41:     static_assert(!ValidUniformReaderFlow<AbsentUniformReader>, \"F7 absence: UniformReader is required\");\n42:     static_assert(!ValidUniformReaderFlow<SharedOnlyUniformReader>, \"F7 borrowed: shared-only UniformReader is invalid\");\n43:     static_assert(!ValidUniformReaderFlow<NonVoidUniformReader>, \"F7 void: non-void UniformReader is invalid\");\n44: }\n45: \n46: TEST(UniformReaderFlowContractTests, FlowRequiresBorrowedVoidUniformReader)\n47: {\n48:     EXPECT_TRUE((ValidUniformReaderFlow<BorrowedUniformFlowComponent>));\n49:     EXPECT_FALSE((ValidUniformReaderFlow<AbsentUniformReader>));\n50:     EXPECT_FALSE((ValidUniformReaderFlow<SharedOnlyUniformReader>));\n51:     EXPECT_FALSE((ValidUniformReaderFlow<NonVoidUniformReader>));\n52: }"
    }
  ],
  "metadata": {
    "truncated": false
  }
}
```

**outil `grep`** :

```json
{
  "status": "completed",
  "input": {
    "limit": 20,
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests",
    "pattern": "LoaderTests"
  },
  "content": [
    {
      "type": "text",
      "text": "Found 20 matches\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\shader\\LoaderTests.cpp:\n  Line 37: class ShaderLoaderTests : public ::testing::Test\n\n  Line 46: TEST_F(ShaderLoaderTests, LoadShaderTest_callCreate)\n\n  Line 61: TEST_F(ShaderLoaderTests, LoadShaderTest_callShaderSource)\n\n  Line 76: TEST_F(ShaderLoaderTests, LoadShaderTest_callCompile)\n\n  Line 91: TEST_F(ShaderLoaderTests, LoadShaderTest_callGetIv)\n\n  Line 106: TEST_F(ShaderLoaderTests, LoadShaderTest_setContextShaderId)\n\n  Line 121: TEST_F(ShaderLoaderTests, LoadShaderFailureTest)\n\n  Line 142: class ShaderLoaderDiagnosticsTests : public ShaderLoaderTests, public ::testing::WithParamInterface<GLint>\n\n  Line 162:         ShaderLoaderTests::TearDown();\n\n\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\src\\graphic\\opengl\\pipeline\\component\\pass\\LoaderTests.cpp:\n  Line 42: class LoaderTests : public ::testing::Test\n\n  Line 58: TEST_F(LoaderTests, LoadPassTest_validContext)\n\n  Line 67: TEST_F(LoaderTests, LoadPassTest_createPipeline)\n\n  Line 76: TEST_F(LoaderTests, LoadPassTest_linkPipeline)\n\n  Line 85: TEST_F(LoaderTests, LoadPassTest_pipelineCreationFailed)\n\n  Line 99: TEST_F(LoaderTests, LoadPassTest_linkFailed)\n\n  Line 121: TEST_F(LoaderTests, LoadPassTest_linkSuccess)\n\n  Line 136: TEST_F(LoaderTests, LoadPassTest_reloadRejectedBeforeAnyNativeCall)\n\n  Line 154: TEST_F(LoaderTests, LoadPassTest_passIDPublishedOnlyAfterLinkSuccess)\n\n  Line 173: TEST_F(LoaderTests, LoadPassTest_attacherFailureReleasesProgramOnce)\n\n  Line 212: class PassLoaderDiagnosticsTests : public LoaderTests, public ::testing::WithParamInterface<GLint>\n\n\n(Results are truncated: showing first 20 results. Consider using a more specific path or pattern.)"
    }
  ],
  "metadata": {
    "matches": 20,
    "truncated": true
  }
}
```

### [85] assistant @ 2026-10-19 07:36:37.872Z — opencode-go/glm-5.3-flash/max

**reasoning** :

Good. Now I have a solid picture. A few remaining checks:

1. Where are Attribute tests for "12 formes/Pointer/IPointer/VBO" — likely `tests/src/graphic/pipeline/AttributeTests.cpp` + `AttributeOwnershipTests.cpp` + GL component `attribute/` tests. Confirmed exist.
2. Check CMake test registration — do tests get globbed or explicitly listed? This matters for H (adding helper files) and A/B/C (adding new test files). Let me grep tests/CMakeLists.txt.
3. Check `Binder.hpp` quickly to confirm VBO creation/publication and no deletion (the doc says so; grep already showed no glDeleteBuffers in include). Maybe quick look at Binder.hpp for setBufferID.
4. Check `Uniform.hpp` set() to confirm "stores value before Setter" claim — grep for "set" in Uniform.hpp. Not strictly necessary; the doc's claims are marked as static observations. I have enough.
5. Check where `opengl/context/UniformContext.hpp` is included — by Setter.hpp (confirmed, L12) and probably by UniformReader via PassContext GL? The GL PassContext likely includes UniformContext/AttributeContext. Let me check quickly the GL PassContext includes. Actually I read the base only. Quick grep on opengl/context/PassContext.hpp.

Let me batch: read tests/CMakeLists.txt (glob first), read opengl/context/PassContext.hpp, read Binder.hpp (maybe 60 lines), grep Uniform.hpp for set.

Derniers contrôles : enregistrement CMake des tests (impact ajout de fichiers pour A/B/C/H), `PassContext` GL (inclusions), `Binder.hpp` (création VBO) et `Uniform::set`.

**outil `glob`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST",
    "pattern": "ARTIST/tests/**/CMakeLists.txt"
  },
  "content": [
    {
      "type": "text",
      "text": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\CMakeLists.txt"
    }
  ],
  "metadata": {
    "count": 1,
    "truncated": false
  }
}
```

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\context\\PassContext.hpp"
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\context\\PassContext.hpp, lines 1-68\n1: // PassContext.hpp\n2: \n3: #pragma once\n4: \n5: #include <GL/glew.h>\n6: #include <graphic/Api.hpp>\n7: #include <graphic/context/PassContext.hpp>\n8: #include <graphic/opengl/context/ShaderContext.hpp>\n9: \n10: namespace artist::graphic\n11: {\n12:     namespace opengl::pipeline::component::pass\n13:     {\n14:         template <auto PROFILE>\n15:         class OpenGLLoader;\n16:         template <auto PROFILE>\n17:         class OpenGLPassFreer;\n18:         template <auto PROFILE>\n19:         class OpenGLShaderAttacher;\n20:         template <auto PROFILE>\n21:         class OpenGLPassUniformReader;\n22:         template <auto PROFILE>\n23:         class OpenGLPassAttributeReader;\n24:         template <auto PROFILE>\n25:         class OpenGLPassUser;\n26:     }\n27: \n28:     namespace opengl::context\n29:     {\n30:         /**\n31:          * @class OpenGLPassContext\n32:          * @brief OpenGL-specific implementation of PassContext.\n33:          *\n34:          * OpenGLPassContext manages the details and state of a shader pass for OpenGL.\n35:          * This includes handling of OpenGL-specific data such as pipeline IDs, as well\n36:          * as the list of shaders and uniform variables.\n37:          */\n38:         class OpenGLPassContext : public graphic::context::PassContext<graphic::api::OpenGL>\n39:         {\n40:         public:\n41:             template <auto PROFILE>\n42:             using Loader = opengl::pipeline::component::pass::OpenGLLoader<PROFILE>;\n43:             template <auto PROFILE>\n44:             using Freer = opengl::pipeline::component::pass::OpenGLPassFreer<PROFILE>;\n45:             template <auto PROFILE>\n46:             using ShaderAttacher = opengl::pipeline::component::pass::OpenGLShaderAttacher<PROFILE>;\n47:             template <auto PROFILE>\n48:             using UniformReader = opengl::pipeline::component::pass::OpenGLPassUniformReader<PROFILE>;\n49:             template <auto PROFILE>\n50:             using AttributeReader = opengl::pipeline::component::pass::OpenGLPassAttributeReader<PROFILE>;\n51:             template <auto PROFILE>\n52:             using User = opengl::pipeline::component::pass::OpenGLPassUser<PROFILE>;\n53: \n54:             void setPassID(GLuint passID)\n55:             {\n56:                 m_passID = passID;\n57:             }\n58: \n59:             GLuint getPassID() const\n60:             {\n61:                 return m_passID;\n62:             }\n63: \n64:         private:\n65:             GLuint m_passID = 0; // OpenGL pipeline ID — 0 means \"no program\"\n66:         };\n67:     }\n68: }"
    }
  ],
  "metadata": {
    "truncated": false
  }
}
```

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\attribute\\Binder.hpp",
    "limit": 120
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\opengl\\pipeline\\component\\attribute\\Binder.hpp, lines 1-36\n1: // file: Binder\n2: \n3: #pragma once\n4: \n5: #include <GL/glew.h>\n6: #include <format>\n7: #include <string>\n8: #include <memory>\n9: #include <any>\n10: #include <graphic/opengl/context/AttributeContext.hpp>\n11: #include <graphic/Api.hpp>\n12: #include <graphic/opengl/profile/Attribute.hpp>\n13: \n14: namespace artist::graphic::opengl::pipeline::component::attribute\n15: {\n16:     template <auto PROFILE>\n17:     class OpenGLBinder\n18:     {\n19:     };\n20: \n21:     template <>\n22:     class OpenGLBinder<graphic::opengl::profile::Attribute::Classic>\n23:     {\n24:     public:\n25:         static void on(typename graphic::api::OpenGL::AttributeContext &attribute)\n26:         {\n27:             if (attribute.getBufferID() == 0)\n28:             {\n29:                 unsigned int VBO;\n30:                 glGenBuffers(1, &VBO);\n31:                 attribute.setBufferID(VBO);\n32:             }\n33:             glBindBuffer(GL_ARRAY_BUFFER, attribute.getBufferID());\n34:         }\n35:     };\n36: }"
    }
  ],
  "metadata": {
    "truncated": false
  }
}
```

**outil `grep`** :

```json
{
  "status": "completed",
  "input": {
    "limit": 15,
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\pipeline\\Uniform.hpp",
    "pattern": "void set|setValue|m_value"
  },
  "content": [
    {
      "type": "text",
      "text": "Found 2 matches\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\include\\graphic\\pipeline\\Uniform.hpp:\n  Line 88:         void set(const T &value)\n\n  Line 93:                 context.template setValue<T>(value);\n"
    }
  ],
  "metadata": {
    "matches": 2,
    "truncated": false
  }
}
```

### [108] assistant @ 2026-10-19 07:36:45.841Z — opencode-go/glm-5.3-flash/max

**reasoning** :

I have enough now. Let me quickly check tests/CMakeLists.txt registration style (explicit list or glob) — important for whether A/B/C/H adding files touches CMake (H claims "Pas de CMake"). Let me read a portion.

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\CMakeLists.txt"
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\CMakeLists.txt, lines 1-25\n1: # CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale\r\n2: # et incluez les sous-projets ici.\r\n3: #\r\n4: cmake_minimum_required (VERSION 3.11)\r\n5: include(GoogleTest)\n6: \r\n7: # Add your test files\r\n8: file(GLOB_RECURSE test_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)\r\n9: include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tests-common/include\r\n10: ${CMAKE_CURRENT_SOURCE_DIR}/include\r\n11: ${CMAKE_CURRENT_SOURCE_DIR}/mock)\r\n12: \r\n13: add_executable(${PROJECT_NAME}_tests ${test_files})\r\n14: \r\n15: target_link_libraries(${PROJECT_NAME}_tests PRIVATE GTest::gmock_main ArtistLib)\n16: artist_copy_asan_runtime(${PROJECT_NAME}_tests)\n17: \r\n18: if(MOCK_GL)\r\n19:         target_compile_definitions(${PROJECT_NAME}_tests PRIVATE __mock_gl__)\r\n20: endif()\r\n21: gtest_discover_tests(${PROJECT_NAME}_tests\n22:     TEST_PREFIX \"unit.\"\n23:     WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}\"\n24:     PROPERTIES LABELS \"unit\"\n25: )"
    }
  ],
  "metadata": {
    "truncated": false
  }
}
```

### [117] assistant @ 2026-10-19 07:36:48.215Z — opencode-go/glm-5.3-flash/max

**reasoning** :

CMake uses GLOB_RECURSE — so adding new test files does NOT require editing CMakeLists.txt, but DOES require reconfiguration (the doc already says "Reconfigurer après ajout de sources"). H's "Pas de CMake" claim is accurate in the sense of no CMakeLists edit, but a reconfigure is needed if H adds a helper .cpp; a header-only helper doesn't even need that (glob only picks .cpp). Good nuance to note.

Also note `MOCK_GL` compile definition `__mock_gl__` — the glFunctionMock macros are conditionally active. That means mocked unit tests and real GL component tests are two different binaries/label sets? Actually there's one test executable with labels "unit"; the OpenGL component tests are in component-tests/ separately (per AGENTS.md). Wait — the tests I found are in ARTIST/tests/src/graphic/opengl/... which is inside the unit test target (GLOB_RECURSE over tests/). The component-tests/ target is separate. So the "opengl" tests under tests/ use the mock via `__mock_gl__` conditional macros. Interesting: so BinderTests etc. run with mocked GL. Real GL tests live in ARTIST/component-tests/. Branch A's "nouveau fichier GL lifetime" test could go either place; the contract K decides. Let me quickly verify component-tests structure to inform the fiches — one glob.

Also confirm: does glFunctionMock only apply under __mock_gl__? Check top of glFunctionMock.hpp for #ifdef. Quick grep.

Dernière vérification : activation conditionnelle du mock (`__mock_gl__`) et séparation des tests GL réels dans `component-tests/`.

**outil `read`** :

```json
{
  "status": "completed",
  "input": {
    "limit": 38,
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\mock\\opengl\\glFunctionMock.hpp"
  },
  "content": [
    {
      "type": "text",
      "text": "Read file C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\tests\\mock\\opengl\\glFunctionMock.hpp, lines 1-38\n1: /**\n2:  * @file glFunctionMock.hpp\n3:  * @brief Mocking framework for OpenGL functions using Google Mock.\n4:  *\n5:  * This header file defines a mock class for OpenGL functions, enabling\n6:  * the simulation and testing of OpenGL-dependent code in isolation.\n7:  * It redefines OpenGL functions to their mock counterparts provided by\n8:  * the `artist::mock::opengl::glFunctionMock` class.\n9:  *\n10:  * Usage:\n11:  * Just include this header in test files to enable the mocking of OpenGL functions.\n12:  * Ensure also that the `__mock_gl__` macro is defined in the test scope.\n13:  * This ensures that OpenGL function calls within the test scope are redirected\n14:  * to the corresponding mock methods, which can be controlled using Google Mock.\n15:  *\n16:  * Limitations:\n17:  * This mocking framework only works if OpenGL functions are used within header files.\n18:  * If OpenGL functions are used directly in cpp files, they are compiled with\n19:  * the actual OpenGL macro definitions, bypassing the mocks.\n20:  *\n21:  * @note This framework is intended for testing purposes only and should not\n22:  * be included in production code.\n23:  *\n24:  * @author Djoé DENNE\n25:  * @version 1.0\n26:  * @date 08/01/2024\n27:  */\n28: \n29: #pragma once\n30: \n31: #ifdef __mock_gl__\n32: \n33: #include <any>\n34: #include <vector>\n35: #include <GL/glew.h>\n36: #include <EnumClass.hpp>\n37: #include <gmock/gmock.h>\n38: \n[Output truncated. Continue reading with offset: 39]"
    }
  ],
  "metadata": {
    "truncated": true
  }
}
```

**outil `glob`** :

```json
{
  "status": "completed",
  "input": {
    "path": "C:\\Users\\djden\\source\\repos\\ARTIST",
    "pattern": "ARTIST/component-tests/src/**/*.cpp"
  },
  "content": [
    {
      "type": "text",
      "text": "C:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\main.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\shader\\UniformReaderComponentTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\shader\\UniformCardinalityComponentTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\shader\\ShaderTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\shader\\ShaderLifetimeComponentTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\shader\\ProgramTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\shader\\PassTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\pipeline\\PipelineLifetimeComponentTests.cpp\nC:\\Users\\djden\\source\\repos\\ARTIST\\ARTIST\\component-tests\\src\\graphic\\pipeline\\AttributeComponentTests.cpp"
    }
  ],
  "metadata": {
    "count": 9,
    "truncated": false
  }
}
```

### [132] assistant @ 2026-10-19 07:36:53.779Z — opencode-go/glm-5.3-flash/max

**reasoning** :

I now have everything needed. Let me also note: `component-tests/src/graphic/pipeline/AttributeComponentTests.cpp` exists — real GL context tests for Attribute. This is relevant for branch A's "régression des 12 formes/Pointer/IPointer/VBO" and the real-GL proof (mocks prove arguments/absence of calls; real GL proves native obligations). The doc says A owns "tests Attribute associés" — I should specify: unit (mock) tests under tests/ AND component-tests AttributeComponentTests.cpp for real-GL proof. Similarly C has UniformCardinalityComponentTests.cpp + UniformReaderComponentTests.cpp in component-tests — relevant to C and B respectively! UniformReaderComponentTests.cpp is a real-GL test of the UniformReader — branch B territory. UniformCardinalityComponentTests.cpp — branch C territory.

This is an ownership nuance to record in fiches: B owns component-tests/shader/UniformReaderComponentTests.cpp (or at least its amendment), C owns component-tests/shader/UniformCardinalityComponentTests.cpp, A owns component-tests/pipeline/AttributeComponentTests.cpp. H: neither LoaderTests has component-tests counterpart (pass/shader loaders are mocked-only? there's ProgramTests, PassTests, ShaderTests in component-tests/shader). Fine.

Also note ShaderLifetimeComponentTests exists — cleanup Shader territory (out of scope, tranche distincte).

Now, scoring validation. The doc's scores:
- P 30→30/35 : lead doc entry. Not a dev task → no 70 floor. Complexity 30 local, risk 35? Risk seems slightly high for a doc pointer, but it carries "HEAD transmis à K/Q" and dirty-file discipline. I'd say 30/30 acceptable; risk ~25-30. I'll validate 30/30 with note, or correct R to 30. Actually risk = severity of failure: a wrong HEAD or scope creep misleads everything downstream. Detectability is high. I'd keep C=30, R=30. Minor correction from 35 → 30 is defensible; but is it worth "correcting"? The doc says validate or correct with justification. I'll validate 30/35 as conservative-but-acceptable? Hmm. Risk of P failing: it's mostly done already ("Archive réalisée et pointeur proposé"). Residual risk: transmitting wrong HEAD/integrated diff. Severity moderate (misleads K/Q), detectability high (HEAD is explicit). I'll correct to 30/30 with justification. Actually — careful: risk shouldn't be derived from priority/confidence. Severity: medium (downstream misdirection). Scope: preparation only. Detectability: high. Ownership: lead. I'll validate at 30 and note 35 was acceptable conservative; final retained: R=30. Hmm, but I shouldn't churn numbers without cause. The instruction: "Corrige les scores si le candidat du doc te semble mal étalonné, en justifiant." P at 35 risk vs 30 — the difference is marginal. I'll validate 30/35 as-is noting it's conservative. Actually let me think again: after P, everything (K, Q, authors) builds on the integrated HEAD. If P transmits a wrong/unclean state (e.g., includes dirty external files in the mission baseline), downstream worktrees inherit confusion. Severity moderate-high, but detectability high and blast radius containable. 35 is fine. Validate.

- K 76→76/78 : engine-architect RO contracts. This is the highest-leverage task: 5 decisions, transversal, determines A/B/C viability. Complexity 76 (transversal contracts, multiple interacting fronts) — validated. Risk 78: if K closes wrong or incompatible with autonomy, branches blocked or silent refonte. High severity, medium detectability (contract review). Validate 76/78.

- Q 60→60/62 : qa-planner RO plan. Not dev → no floor. Validate 60/62.

- M 45→70/58 : cpp-engineer, mock extension provision. Dev task → floor 70 applies: retained 70 ✓ (45→70 correct). Risk 58: glFunctionMock.hpp is included by 20 test files across A/B/C/H — a breaking change to the mock breaks everyone. Singleton, macro-based redefinition. Ownership conflict risk is real (doc already flags it). Validate 70/58. Evidence: 20 files include glFunctionMock (I counted 20 matches across tests/src). Also M is conditional: only if K/Q reveal need; must integrate before consumers.

- A 82→82/86 : gpu-engineer VBO cycle. Complexity 82: GL lifetime, ownership/move/rebind/release, exact-once, context-bound, new lifetime file, regression across 12 formes + component tests. Validate. Risk 86: GPU resource leak/double-delete, moved-from state, context loss — high severity, hard detection (glIsBuffer insufficient). Validate 82/86.

- B 78→78/80 : cpp-engineer reflection publication. Owns BOTH PassContext files (base+GL) + 2 Readers + tests. Complexity 78 validated (transactionality decisions, borrowed invalidation, historical publication tests). Risk 80: PassContext base is included by User/Freer/Loader/ShaderAttacher (4 other components) — changes ripple; contract amendments to publication guarantee. Validate 78/80.

- C 68→70/72 : cpp-engineer uniform refusal. Dev → floor: 70 ✓. Risk 72: CPU state after refusal, cardinality matrix, no silent contract widening. Setter.hpp is a big template surface (25+ specializations). Validate 70/72.

- H 56→70/55 : build-engineer fixture isolation. Dev-ish (test code) → floor 70 ✓. Risk 55: fixtures only, but shuffle/permutation isolation can mask production errors if done badly — the doc warns "sans masquer les erreurs de production". Risk of masking = severity moderate; detectability medium. Validate 70/55. Note: historical incidents not reproduced on this candidate — H may end in characterization without patch (constat sans patch). Fine.

- I 64→70/76 : lead integration. Dev → floor 70 ✓. Risk 76: cross-branch interactions (VBO replacement × reflection, Uniform refusal × upload), merge conflicts, serialization of builds, GPU bail. Validate 70/76.

- G 58→58/60 : test-engineer RO gap proposals after I. Not dev (text proposals) → no floor; 58 retained. Validate.

- F 62→70/65 : provision for accepted-gap corrections (test code → floor 70). Provisional, not published generically. Validate 70/65.

- R 66→66/78 : correctness-reviewer fresh RO after I. Not dev → no floor. Risk 78: lifetime/UB review findings invalidate conclusions; high severity if missed. Validate.

- V 70→70/77 : test-reviewer fresh, off-board, final audit. Not author → hmm, is V a dev task? No — audit. So floor doesn't apply; 70 brute is its own estimate. Validate 70/77.

So my corrections: essentially none needed on the floors (doc already applied them correctly), minor validation notes. I might adjust M risk upward? 58 → given 20 includer files, a mock signature change (e.g., adding glDeleteBuffers mock) touches a singleton used by all; but M is bounded to additive extension with RED/GREEN local. 58 is reasonable; I could argue 60. Keep 58 validated with note.

One real correction candidate: the doc's H says only the two LoaderTests.cpp files owned. I verified pass/LoaderTests.cpp and shader/LoaderTests.cpp exist. ✓. And "helper neuf local si nécessaire" — GLOB_RECURSE picks *.cpp automatically; a header helper needs no CMake change; a new .cpp needs reconfigure. Note in fiche.

Écarts doc vs code réel (item 5 of deliverables):
1. PassContext ownership: doc says B owns "context/PassContext.hpp" — actual: TWO files must be owned by B: `ARTIST/include/graphic/context/PassContext.hpp` (base, addUniform L51/addAttribute L67) AND `ARTIST/include/graphic/opengl/context/PassContext.hpp` (OpenGLPassContext subclass, publication path via openglContext.addAttribute/addUniform). Already integrated per the user's message — I confirm and make it explicit in fiche B.
2. B's Readers: AttributeReader.hpp includes both `graphic/opengl/context/PassContext.hpp` AND `graphic/opengl/context/AttributeContext.hpp` (L14) — so B reads (not owns) AttributeContext (owned by A). Interface freeze via K needed: if A changes AttributeContext/Attribute lifecycle, AttributeReader compilation is affected. Cross-branch compile dependency A→B at integration. UniformReader.hpp includes only PassContext GL — no direct UniformContext include; UniformContext reached via Setter.hpp (C) and base PassContext stores Uniforms. So no header-level B↔C conflict, but semantic interaction remains (Uniform refusé × réflexion).
3. component-tests ownership not mentioned in the doc: A's real-GL regression lives in `ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp`; B has `component-tests/src/graphic/shader/UniformReaderComponentTests.cpp`; C has `UniformCardinalityComponentTests.cpp`. The doc's branch fiches mention only tests/ files. These component-tests files must be assigned (A/B/C respectively) to avoid no-man's-land or double ownership. This is a genuine gap I should flag and integrate into fiches.
4. Mock consumers: 20 test files include glFunctionMock.hpp across all four branches' test files (A: Binder/Setter/Unbinder/AttributeOwnership; B: AttributeReader/UniformReader tests; C: Uniform* tests; H: both LoaderTests) → confirms M-before-authors barrier and the "pas de mock commun" constraints of A/B/C/H (they include but must not modify it).
5. glDeleteBuffers/glIsBuffer absent from ARTIST/include — grep confirms zero matches → A's RED (release) will require adding the deletion path; the observation holds.
6. CMake: tests registered via GLOB_RECURSE (tests/CMakeLists.txt:8) → new test files need reconfigure, not CMake edit; H's "pas de CMake" holds but reconfigure reminder applies. Mock is active only under `__mock_gl__` (MOCK_GL option, tests/CMakeLists.txt:18-20; glFunctionMock.hpp:31) — unit GL-mocked tests vs component-tests (real GL) are distinct proof tiers; K must specify which tier proves each obligation (mock proves call arguments/absence; real GL proves native behavior) — consistent with doc.
7. Binder.hpp confirmed: creates VBO if getBufferID()==0, binds; no deletion anywhere (Binder.hpp:27-33). Attribute ownership replacement in base PassContext (PassContext.hpp:67-74) immediate map assignment — old owner destroyed silently at assignment (unique_ptr replaced) — this is the exact "remplacement d'owner" K#4 concerns. Note: `m_attributes[name] = std::move(attribute)` destroys previous Attribute → if Attribute becomes VBO owner, replacement triggers glDeleteBuffers under whatever context is current at that moment. This is precisely the interaction to contract. Good evidence: context/PassContext.hpp:73.
8. Uniform::set stores value then Setter (Uniform.hpp:88-93) — confirms C's observation; Setter rejects cardinality (Setter.hpp:21-30 requireSingleValue throws before GL) — note: requireSingleValue rejects BEFORE GL call but AFTER Uniform::set stored the value → CPU state divergence already observable. Confirms C's RED premise. 

Also the doc says "Les Readers ... publient dans leur boucle" via openglContext.addAttribute (L100) / addUniform (L97) — user verified. I trust user's line numbers; I saw includes at L9/L14 of AttributeReader. Good enough.

Graphe avec barrières:
- P → (barrière: HEAD intégré transmis) → K (RO) → décisions humaines/lead (K fermées) → Q (RO) → barrière: contrats+plan QA persistés et commités → [M si nécessaire, intégré] → barrière intégration M → A ∥ B ∥ C ∥ H → barrière I (shutdown/merge réel des 4 branches, diff relu) → G ∥ R (fresh, RO) → corrections bornées F (réévaluées) → nouvelle intégration → candidat final intégré → V (fresh, hors board) → validation lead → documentation.

Consommateurs hors board jusqu'à intégration:
- A/B/C/H restent hors board tant que K/Q ne sont pas persistés/commités (leurs worktrees n'existent pas avant).
- M intégré avant que A/B/C/H démarrent (pas en parallèle avec eux).
- G/R publiés seulement après I (merge réel), pas après la fin du dernier auteur.
- F publié seulement après tri G (KEEP/MODIFY/REJECT/DEFER) avec chemins/owner réels.
- V publié seulement après intégration du candidat final (post F/rectifications).

Déclencheur QA: risque PM ≥ 50 pour A(86), B(80), C(72), I(76), K(78), V(77), R(78), M(58? no <50... wait 58 ≥ 50 yes), H(55 ≥ 50), F(65 ≥ 50), Q(62 ≥ 50)... hmm risk ≥ 50 triggers the QA loop. Per AGENTS.md: "Boucle QA obligatoire pour un comportement sous contrat OU une suite d'intégration étendue OU un risque PM >= 50 : qa-planner (Q) avant les auteurs, plan persisté/commité par le lead avant leurs worktrees ; auteurs TDD ; producteurs réellement intégrés avant test-engineer ; test-reviewer fresh hors board". Here: comportement sous contrat (OUI — toute la mission porte des contrats de lifetime/publication/refus) ET risque ≥ 50 sur plusieurs fiches → les DEUX déclencheurs sont réunis. Confirm Q before authors, G after I, V fresh. QA I×P ≥ 10 products: Q produces the matrix for the mission's products (A/B/C/H/I primarily) with déclencheur/blast radius/détection/mitigation/gap for products ≥ 10.

Décisions K (5) with what blocks what and what remains human:
1. VBO owner/adoption/borrow of setBufferID, rebind, release, exact-once, move loaded→loaded, destruction/replacement in map, moved-from, context/thread, no-context. Blocks A entirely (RED cannot be written without the contract) and partially B (replacement semantics in PassContext::addAttribute) and K#4. Human: adopt the guarantee set; if it demands global architecture → stop and return to lead (réduction au lot VBO possible).
2. Réflexion atomicité (per-Reader vs per-Pass), ancien état en échec, collection vide/noms disparus, invalidation des emprunts. Blocks B. Human: amendement de contrat public (le test de publication historique peut être amendé explicitement).
3. Uniform état CPU après rejet, matrice CPU/GLSL (bool/samplers/entiers), programme courant, doubles/extensions. Blocks C. Human: no silent widening/narrowing.
4. Interactions PassContext lors du remplacement d'un Attribute devenu owner VBO ; interfaces de métadonnées stables entre B/C ; séparer fichiers auteur ou sérialiser la frontière. Blocks A∩B (PassContext base owned by B touched by A's semantics) and B∩C metadata. Human/lead: ownership arbitration if not separable.
5. Dette Attribute-GL: audit final ouvert, candidat + baseline courants avant mutation, FAIL causal traité ou arbitré avant consommation. Blocks all authors (baseline discipline) and V (audit scope). Human: arbitrage des FAIL.

Preparation-only confirmation: no publication, no team, no workers, no source writes; /ensemble-mission cycle initiated by user; scores transmitted by orchestrator via team_tasks_add later.

Now the fiches. Format: numbered, id, agent, owned files (real paths), deps (real barriers), scores brute→retenue C/R, context, observable criteria + RED/GREEN expected, exclusive resources, worktree flag, estimation (the doc has budget; fiches in doc don't have per-task hour estimates — I'll give coarse estimates consistent with budget 32–50 h productive).

Let me be compact but complete. French output. No file dumps.

Worktree: authors A/B/C/H/M(true), F(true), lead tasks P/I(false — lead works in main tree), RO readers K/Q/G/R/V (worktree: false, lecture seule).

Context: fresh for all (per contract). 

Estimations (h-personne, indicative):
- P: 1-2h. K: 6-9h. Q: 3-5h. M: 0-4h (conditional). A: 10-14h. B: 8-12h. C: 7-10h. H: 5-8h. I: 6-9h. G: 3-4h. F: 0-6h conditional. R: 3-5h. V: 4-6h. Sum productive ≈ 33-50h+ — aligns with the doc budget (32-50). Good.

Now write the plan. Compact. Numbered fiches. Graph in text. Then écarts/risques. Then preparation-only confirmation.

One more check I should do: confirm AttributeReader L100 addAttribute and UniformReader L97 addUniform (user gave these; trust). And Attribute.hpp path exists (glob confirmed). Uniform.hpp class Uniform L51 — glob confirmed file; user gave line. Fine.

Also verify tests for attribute "12 formes": AttributeTests.cpp + AttributeOwnershipTests.cpp + AttributeContractTests.cpp + GL attribute/{Binder,Setter,Unbinder}Tests + component AttributeComponentTests — I'll reference them collectively as "tests Attribute existants" without counting.

Scoring justifications must be short. Let me also explicitly state method PM/LLM (session model GLM, no imposed model), no JEV.

For each fiche: objectif/livrable ; type ; profil ; entrées/chemins possédés ; contrat/invariants ; critères et commandes de vérification ; dépendances ; ressources exclusives ; décision de contexte ; estimation. That's a lot per fiche — keep each line terse.

Vérification commands: unit tests via ctest labels (from doc), reconfigure after adding sources. For K/Q/G/R/V (RO) verification = deliverable review, no build.

Écriture du plan final maintenant.

# Plan de mission validé — « Frontières déterministes des ressources OpenGL »

Méthode d'évaluation : **PM/LLM** (modèle de session, aucun provider/modèle imposé), heuristiques C et R indépendantes sur 100, plancher dev `max(70, brute)`. Lecture seule effectuée sur HEAD cd1b598 + greps/globs du code réel. Aucune écriture, aucune équipe, aucune publication.

---

## 1. Fiches de tâches

### F1 — P (entrée documentaire)
- **Agent** : lead. **Type** : doc. **Contexte** : fresh. **Worktree** : false.
- **Possédé** : `docs/architecture/next-mission.md` (mise à jour du statut après validation), transmission du HEAD intégré.
- **Entrées** : archive ingestion Attribute-GL, rétrospective `research/`, HEAD cd1b598.
- **Critères** : HEAD intégré identifié et transmis à K/Q ; docs canoniques hors ownership intacts ; aucune mention de scope moteur commencé.
- **Déps** : aucune. **Ressources exclusives** : néant (RO sur le reste).
- **Scores** : C 30→30 / R **35→35 validé** (un HEAD/état mal transmis désoriente K/Q/A/B/C ; détection facile mais amplitude en aval). **Estimation** : 1–2 h.

### F2 — K (contrats et 5 décisions)
- **Agent** : engine-architect (RO). **Type** : contrat. **Contexte** : fresh. **Worktree** : false.
- **Possédé** : livrable texte (ledger options/états/erreurs/alternatives + garanties retenues). Ne modifie aucune source.
- **Entrées** : P (HEAD), code observé (`Binder.hpp:27-33` crée/publie un VBO sans delete ; `context/PassContext.hpp:67-74` remplacement immédiat d'owner par assignation map — destruction silencieuse du précédent `unique_ptr` ; `Uniform.hpp:88-93` stocke la valeur avant Setter ; `uniform/Setter.hpp:21-30` refus cardinalité **avant** GL mais **après** stockage CPU).
- **Invariants à trancher** : les 5 décisions K (voir §4). Toute garantie nouvelle est adoptée explicitement, sans promouvoir d'ADR Proposed.
- **Critères** : chaque décision a options, décision, oracle associable, et déclaration des fichiers/interfaces gelées pour A/B/C ; si une garantie exige une refonte globale → stop, retour au lead (option réduction au lot VBO conservée).
- **Déps** : F1 (barrière : HEAD transmis). **Débloque** : Q puis tous les auteurs.
- **Scores** : C 76→76 / R 78 — **validés** (contrats transversaux sur lifetime GPU, publication et refus ; un contrat incompatible avec l'autonomie bloque toute la mission). **Estimation** : 6–9 h.

### F3 — Q (plan QA)
- **Agent** : qa-planner (RO). **Type** : QA planning. **Contexte** : fresh (requis). **Worktree** : false.
- **Possédé** : plan QA persisté/commité par le lead **avant tout worktree d'auteur** : exigences nominales/négatives, invariants, top 3 risques, correspondance preuve↔obligation, orphelins, matrice QA I×P (1–5 → 1–25) avec déclencheur/blast radius/détection/mitigation/gap pour tout produit ≥ 10.
- **Critères** : chaque obligation K a un niveau de preuve assigné (mock = arguments/absence d'appels ; GL réel = obligation native ; `glIsBuffer` seul ne prouve ni exact-once ni retraite).
- **Déps** : F2 **validé** (barrière : décisions fermées). **Débloque** : commit contrats+QA, puis M/A/B/C/H.
- **Scores** : C 60→60 / R 62 — **validés** (non-dev, pas de plancher). **Estimation** : 3–5 h.

### F4 — M (entrées mock communes, conditionnelle)
- **Agent** : cpp-engineer. **Type** : dev (plancher appliqué). **Contexte** : fresh. **Worktree** : true.
- **Possédé** : `ARTIST/tests/mock/opengl/glFunctionMock.hpp` **uniquement** (si extension nécessaire, ex. mock `glDeleteBuffers`/`glGenBuffers` manquants).
- **Fait vérifié** : **20 fichiers de tests incluent ce mock singleton** (A : Binder/Setter/Unbinder/AttributeOwnership ; B : AttributeReader/UniformReader ; C : Uniform* ; H : les deux LoaderTests) → le moindre changement de signature casse les quatre branches. Active seulement sous `__mock_gl__` (`glFunctionMock.hpp:31`, option `MOCK_GL` `tests/CMakeLists.txt:18-20`).
- **Contrat** : extension additive, RED/GREEN local au double, comportements historiques préservés ; pas le RED métier des auteurs.
- **Critères** : si K/Q ne requièrent aucune extension → fiche retirée, pas de tâche factice.
- **Déps** : K/Q commités ; **intégration réelle de M avant le démarrage de A/B/C/H** (jamais en parallèle avec eux).
- **Scores** : C 45→**70** (plancher) / R 58 — **validés** (risque réel via singleton partagé, borné par l'additivité). **Estimation** : 0–4 h.

### F5 — A (cycle VBO)
- **Agent** : gpu-engineer. **Type** : dev. **Contexte** : fresh. **Worktree** : true.
- **Possédé** : `ARTIST/include/graphic/pipeline/Attribute.hpp`, `graphic/context/AttributeContext.hpp`, `graphic/opengl/context/AttributeContext.hpp`, `graphic/opengl/profile/Attribute.hpp`, `graphic/opengl/validator/AttributeConcept.hpp`, `graphic/opengl/pipeline/component/attribute/` (Binder/Setter/Unbinder), **nouveau fichier GL lifetime**, tests : `tests/src/graphic/pipeline/Attribute{Tests,OwnershipTests,ContractTests}.cpp`, `tests/src/graphic/context/AttributeContextTests.cpp`, `tests/src/graphic/opengl/pipeline/component/attribute/{Binder,Setter,Unbinder}Tests.cpp`, `component-tests/src/graphic/pipeline/AttributeComponentTests.cpp`.
- **Interdits** : PassContext (les 2), Uniform*, Readers, mock commun (include OK, écriture interdite).
- **Contrat/RED** : release exact-once, move chargé→chargé, rebind, replacement en map, moved-from, contexte courant — selon K. Aucun `glDeleteBuffers` n'existe dans `ARTIST/include` (grep : 0 match) → le RED release est nouveau code, pas une régression.
- **GREEN** : implémentation minimale + non-régression 12 formes/Pointer/IPointer/VBO (mock) + obligations natives en contexte GL réel (component-tests).
- **Vérif** : reconfigurer (GLOB_RECURSE, `tests/CMakeLists.txt:8`) puis `ctest -L unit` et `ctest -L opengl`.
- **Déps** : contrats/QA commités + M intégré. **Débloque** : I.
- **Scores** : C 82→82 / R 86 — **validés** (lifetime GPU, double-delete/leak peu détectables, `glIsBuffer` insuffisant). **Estimation** : 10–14 h.

### F6 — B (publication réflexion)
- **Agent** : cpp-engineer (distinct de C). **Type** : dev. **Contexte** : fresh. **Worktree** : true.
- **Possédé** : **les deux** `ARTIST/include/graphic/context/PassContext.hpp` (base : `addUniform` L51, `addAttribute` L67) et `ARTIST/include/graphic/opengl/context/PassContext.hpp` (sous-classe, chemin de publication), `graphic/opengl/pipeline/component/pass/AttributeReader.hpp` (publie via `openglContext.addAttribute` L100), `pass/UniformReader.hpp` (`addUniform` L97), tests : `tests/src/graphic/opengl/pipeline/component/pass/{AttributeReader,UniformReader}Tests.cpp`, `tests/src/graphic/context/PassContextTests.cpp`, `component-tests/src/graphic/shader/UniformReaderComponentTests.cpp`, fichier d'intégration dédié.
- **Interdits** : contextes Attribute/Uniform, mock commun. **Lit** `AttributeContext` (`AttributeReader.hpp:14`) sans le posséder → gel d'interface K requis.
- **Contrat/RED** : première entrée valide puis seconde invalide avec ancien état identifiable ; succès/échec tardif/vide/noms disparus/invalidation d'emprunts selon K. Le test de publication historique ne s'affaiblit pas ; tout changement = amendement explicite.
- **Attention ripple** : la base PassContext est incluse par `pass/{Loader,Freer,User,ShaderAttacher}.hpp` — changements compatibles ou review I.
- **Déps** : contrats/QA commités + M intégré. **Débloque** : I.
- **Scores** : C 78→78 / R 80 — **validés** (atomicité transactionnelle, frontière publique amendée, 4 consommateurs directs de la base). **Estimation** : 8–12 h.

### F7 — C (refus Uniform)
- **Agent** : cpp-engineer (distinct de B). **Type** : dev. **Contexte** : fresh. **Worktree** : true.
- **Possédé** : `graphic/pipeline/Uniform.hpp`, `graphic/context/UniformContext.hpp`, `graphic/opengl/context/UniformContext.hpp`, `graphic/opengl/pipeline/component/uniform/Setter.hpp`, **nouveau fichier GL dédié**, tests : `tests/src/graphic/pipeline/Uniform{Tests,OwnershipTests,CardinalityTests,ValueTests,ContractTests}.cpp`, `component-tests/src/graphic/shader/UniformCardinalityComponentTests.cpp`.
- **Interdits** : Readers, PassContext, mock commun.
- **Contrat/RED** : via API publique — rejet cardinalité/compatibilité avec ancienne valeur CPU observable selon K (constat confirmé : `Uniform::set` stocke avant Setter, `Setter.hpp:21-30` rejette avant GL → divergence CPU déjà possible), zéro upload au refus, succès par famille réellement autorisée. Pas de compatibilité GLSL universelle inventée.
- **Déps** : contrats/QA commités + M intégré. **Débloque** : I.
- **Scores** : C 68→**70** (plancher) / R 72 — **validés** (surface template Setter ~25 spécialisations, matrice CPU/GLSL délicate, risque de masquage par tolérance). **Estimation** : 7–10 h.

### F8 — H (isolation fixtures Loader)
- **Agent** : build-engineer. **Type** : dev (tests). **Contexte** : fresh. **Worktree** : true.
- **Possédé** : **uniquement** `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/LoaderTests.cpp` et `ARTIST/tests/src/graphic/opengl/pipeline/component/shader/LoaderTests.cpp` + helper neuf local header si nécessaire.
- **Interdits** : Loader production (`pass/Loader.hpp` existe — non possédé), CMake, mock global.
- **Contrat/RED** : seeds historiques, permutations inversées sur candidat identifié, contrôle négatif conservé ; si la suspicion ne se reproduit pas → caractérisation sans patch artificiel (constat sans patch est un livrable valide).
- **Note CMake** : GLOB_RECURSE → un helper `.cpp` nouveau exige reconfigure, pas d'édition CMakeLists ; un helper header-only n'exige rien.
- **Déps** : contrats/QA commités + M intégré. **Débloque** : I.
- **Scores** : C 56→**70** (plancher) / R 55 — **validés** (risque principal : masquer une erreur de production par une correction de fixture — interdit par contrat). **Estimation** : 5–8 h.

### F9 — I (intégration et interactions)
- **Agent** : lead + propriétaires. **Type** : intégration. **Contexte** : fresh. **Worktree** : false (arbre principal).
- **Possédé** : merges des branches A/B/C/H ; aucun fichier source possédé en propre — tout code de résolution **retourne à son owner** avec sa propre tranche TDD.
- **Contrat** : barrière = A/B/C/H réellement shutdown **et merge** (pas « fin d'auteur »), diff relu, puis tests croisés : remplacement VBO × réflexion (PassContext), Uniform réfléchi × upload. Toute nouvelle fixture reçoit ownership + tranche. Builds sérialisés par arbre de sortie ; un seul bail GPU.
- **Vérif** : `ctest -L unit` + `ctest -L opengl` complets, comptages comparés à la baseline du candidat.
- **Déps** : F5+F6+F7+F8 intégrés. **Débloque** : G ∥ R.
- **Scores** : C 64→**70** (plancher, code de résolution possible) / R 76 — **validés** (interactions A∩B dans PassContext, conflits de merge, sérialisation builds/GPU). **Estimation** : 6–9 h.

### F10 — G (gaps de tests)
- **Agent** : test-engineer (RO). **Type** : QA. **Contexte** : fresh (requis). **Worktree** : false.
- **Possédé** : propositions texte avec oracles et verdict KEEP/MODIFY/REJECT/DEFER. Pas de production, pas de runner autonome, pas le RED des auteurs. Le lead persiste et exécute.
- **Déps** : **I terminé** (barrière : candidat réellement intégré). **Débloque** : F (provision).
- **Scores** : C 58→58 / R 60 — **validés** (non-dev). **Estimation** : 3–4 h.

### F11 — F (corrections de gaps acceptés — provision)
- **Agent** : à déterminer sur les vrais gaps (owner des fichiers touchés). **Type** : dev (code de test → plancher). **Contexte** : fresh. **Worktree** : true.
- **Contrat** : **ne pas publier génériquement** — chemins, owner et scores réévalués après tri G ; correction production renvoyée à A/B/C/H ; nouvelle intégration avant V.
- **Scores** : C 62→**70** (plancher) / R 65 — **validés**. **Estimation** : 0–6 h.

### F12 — R (revue lifetime)
- **Agent** : correctness-reviewer, **fresh**, hors board d'implémentation. **Type** : revue. **Contexte** : fresh (requis). **Worktree** : false.
- **Possédé** : findings avec contrat/diff/critères observables ; corrections par les propriétaires ; conclusions affectées par une mutation invalidées et ré-auditées.
- **Déps** : **I terminé**. Parallèle à G.
- **Scores** : C 66→66 / R 78 — **validés** (lifetime/UB/remplacement d'owner : gravité élevée, détectabilité moyenne). **Estimation** : 3–5 h.

### F13 — V (audit final)
- **Agent** : test-reviewer, **fresh, hors board d'implémentation**. **Type** : audit. **Contexte** : fresh (requis). **Worktree** : false.
- **Possédé** : audit contrat + preuves du candidat final intégré (pas les justifications d'auteurs). Scénario discriminant avant tout finding haut ; retour propriétaire → réintégration → ré-audit. `rendering-verifier` en renfort si preuve GPU ambiguë ; pas de nouveau coordinateur.
- **Déps** : **candidat final intégré** (après I et corrections F/R absorbées).
- **Scores** : C 70→70 / R 77 — **validés** (audit = estimation propre, pas de plancher ; verdict global conditionne la sortie de mission). **Estimation** : 4–6 h.

---

## 2. Graphe d'exécution et barrières de publication

```text
P (HEAD transmis)
  └─▶ K (contrats, RO) ──▶ décisions humaines/lead fermées (§4)
        └─▶ Q (plan QA, RO)
              └─▶ BARRIERE 1 : contrats + plan QA persistés ET commités
                    └─▶ [M si nécessaire] ──▶ BARRIERE 2 : M intégré
                          └─▶ A ∥ B ∥ C ∥ H   (worktrees isolés, 1 écrivain/fichier,
                                │              builds sérialisés, 1 bail GPU)
                                └─▶ BARRIERE 3 : I — shutdown+merge RÉELS des 4 branches,
                                      diff relu, tests croisés (pas « fin d'auteur »)
                                      └─▶ G ∥ R (fresh, RO)
                                            └─▶ corrections bornées (F, réévaluées)
                                                  └─▶ BARRIERE 4 : candidat final intégré
                                                        └─▶ V (fresh, hors board)
                                                              └─▶ validation lead → documentation
```

**Consommateurs maintenus hors board** : A/B/C/H/M tant que contrats+QA ne sont pas commités (un worktree ne reçoit pas le dirty implicite — le HEAD intégré par P est la seule entrée) ; G et R ne sont pas publiés avant le merge réel de I ; F n'est publié qu'après le tri G avec chemins/owner réels ; V n'est publié qu'après l'intégration du candidat final. Aucune tâche d'attente factice.

## 3. Déclencheur QA — confirmé

Les deux déclencheurs sont réunis simultanément : (a) **comportement sous contrat** — toute la mission définit des garanties de lifetime/publication/refus ; (b) **risque PM ≥ 50** sur K (78), M (58), A (86), B (80), C (72), H (55), I (76), F (65), R (78), V (77). Donc : **Q avant tout auteur**, plan persisté/commité par le lead **avant la création des worktrees d'auteurs** ; auteurs TDD ; producteurs réellement intégrés avant G ; **V fresh hors board d'implémentation**. Produits QA I×P ≥ 10 (à établir par Q) exigent déclencheur, blast radius, détection, mitigation et gap — sans conversion du risque PM (0–100) en matrice QA (1–25).

## 4. Décisions K à fermer avant les auteurs

1. **VBO** — owner/adoption/emprunt de `setBufferID`, rebind, release exact-once, move chargé→chargé, remplacement en map (destruction du précédent owner à `context/PassContext.hpp:73`), moved-from, contexte/thread, cas sans contexte. **Bloque** A entièrement (RED impossible sans contrat) et B partiellement. **Humain/lead** : adoption du jeu de garanties ; si refonte globale → arrêt et réduction possible au lot VBO seul.
2. **Réflexion** — atomicité par Reader ou Pass, ancien état en échec, collection vide/noms disparus, moment d'invalidation des emprunts. **Bloque** B. **Humain** : amendement explicite du contrat de publication historique.
3. **Uniform** — état CPU après rejet (constat : stockage avant Setter, refus avant GL), matrice CPU/GLSL (bool/samplers/entiers), programme courant, doubles/extensions. **Bloque** C. **Humain** : aucun élargissement/retrait silencieux.
4. **Interactions** — responsabilité de PassContext quand un Attribute devenu owner VBO est remplacé ; interfaces de métadonnées stables B↔C ; séparation des fichiers ou sérialisation de la frontière. **Bloque** A∩B (base PassContext possédée par B, sémantique portée par A) et B∩C. **Lead** : arbitrage d'ownership si non séparable.
5. **Dette Attribute-GL** — audit final ouvert ; candidat + baseline courants établis avant toute mutation ; tout FAIL causal dans les mêmes frontières traité ou arbitré avant consommation, jamais absorbé par un comptage global. **Bloque** tous les auteurs (discipline de baseline) et le périmètre de V. **Humain** : arbitrage des FAIL.

## 5. Écarts détectés doc ↔ code réel

- **Chemins abrégés confirmés** (intégrés aux fiches) : les deux PassContext (base + GL) pour B ; UniformContext base + GL pour C ; `AttributeReader.hpp:14` inclut `opengl/context/AttributeContext.hpp` → dépendance de **lecture** B vers un fichier possédé par A, à geler via K.
- **Tests component-tests non assignés dans le doc** : `component-tests/src/graphic/pipeline/AttributeComponentTests.cpp` (→ A), `component-tests/src/graphic/shader/UniformReaderComponentTests.cpp` (→ B), `UniformCardinalityComponentTests.cpp` (→ C). Sans assignation, ces preuves GL réelles restent en no-man's-land. Corrigé dans F5/F6/F7.
- **Mock singleton** : 20 fichiers de tests l'incluent, à travers les quatre branches → la barrière « M intégré avant les auteurs » n'est pas une prudence mais une nécessité mécanique (tout changement non additif casse les branches en parallèle).
- **CMake** : `GLOB_RECURSE` (`tests/CMakeLists.txt:8`) — « pas de CMake » pour H est exact, mais un ajout de `.cpp` exige une reconfiguration ; mock actif seulement sous `__mock_gl__` (`MOCK_GL`, lignes 18–20) → hiérarchie de preuves mock/GL-réel à verrouiller dans Q.
- **Observations statiques du doc confirmées** : zéro `glDeleteBuffers`/`glIsBuffer` dans `ARTIST/include` ; `Binder.hpp:27-33` crée/publie sans libération ; `PassContext.hpp:67-74` remplace l'owner immédiatement ; `Uniform.hpp:88-93` + `Setter.hpp:21-30` confirment la divergence CPU possible avant refus.

## 6. Périmètre — préparation seule, confirmé

Ce plan reste **en lecture seule** : aucune publication de tâche, aucune création d'équipe, aucun worker, aucun build, aucune écriture de source ou de doc. Les scores ci-dessus (deux entiers 0–100 par fiche, méthode PM/LLM) sont transmis à l'orchestrateur pour `team_tasks_add` après validation — sans classification automatique. Le cycle natif (`/ensemble-mission config --mission-mode long_run`, présentation de la politique, `/ensemble-mission start`) est initié **par l'utilisateur seul** ; l'approbation de ce plan ne vaut ni `start` ni levée de pause opérateur. Dirty externe préservé, hors périmètre, jamais exigé en commit.

**Chemins critiques** : K → décisions → Q → commit → (M) → A/B/C/H → I → V. Le gain du parallélisme est limité par la fermeture des décisions K ; la sortie de secours (réduction au lot VBO) reste ouverte si K#2/#3 n'aboutissent pas sans refonte globale.

### [139] idle @ 2026-10-19 07:38:28.579Z

```json
{
  "time": {
    "created": 1792395508579
  },
  "outcome": "succeeded"
}
```

