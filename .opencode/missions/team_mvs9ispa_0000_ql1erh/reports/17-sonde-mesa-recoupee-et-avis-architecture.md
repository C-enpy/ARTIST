# QA Linux — sonde Mesa recoupée, avis de correction demandé

## Résultat décisif

La sonde comparative a été exécutée après lecture du brief local et ses preuves ont été relues par le lead. Elle invalide l'hypothèse d'une distinction scalaire/tableau mono-élément par la requête de localisation `[0]` sur le pilote testé. **Aucun correctif du moteur n'est engagé.**

Les trois programmes GLSL330 sont actifs, compilent et lient :
- L'ancienne réflexion renvoie le même nom nu `qaProbe` pour le scalaire et les tableaux1/2, avec tailles1/1/2.
- Scalaire et tableau1 donnent tous deux base0 et `[0]`0 ; `[1]`, `[2]` et nom absent donnent-1. Cette requête ne les discrimine donc pas ici.
- Le tableau2 donne `[1]`1, contrôle de cardinalité attendu.
- La réflexion moderne distingue les déclarations : `qaProbe` pour le scalaire, `qaProbe[0]` pour les tableaux1/2.

Le contexte obtenu est Mesa25.2.8/llvmpipeLLVM20.1.2, OpenGL4.5core demandé3.3core. L'extension est annoncée et les fonctions modernes sont chargées. Toutes les requêtes rapportent erreurGL0 et la terminaison des noms legacy est correcte. Pas de draw, d'upload ou de mutation de buffer dans cette expérience.

## Ce que cela prouve et ne prouve pas

La divergence entre les deux routes de réflexion est désormais **observée sur le même binaire**, et non plus seulement suggérée par le code d'un miroir Mesa. Elle fournit une information discriminante disponible sur cet environnement.

Elle ne prouve pas un comportement universel des pilotes3.3, ne répare pas le pilote et ne démontre pas une mutation indue par ARTIST. La classification par localisation `[0]` est écartée ; elle ne sera pas transformée en fallback silencieux. Les trois échecs des suites normale/ASan restentFAIL.

## Avis d'architecture avant auteur

Le lead a publié une tâche fresh en lecture seule à l'architecte, après mise à disposition des preuves finalisées dans l'arbre lead. L'avis doit déterminer si l'information moderne peut servir uniquement à la classification interne lorsque disponible, tout en conservant les clés natives legacy et le chemin3.3 déjà qualifié. C'est une piste à examiner, pas une décision adoptée.

L'avis doit expliciter le gap des environnements3.3 non qualifiés et distinguer correction compatible de changement public : alias, canonicalisation des clés, nouvelle provenance source/cook, rejet supplémentaire ou relèvement du minimum backend ne sont pas autorisés implicitement. Toute frontière indispensable hors mandat sera présentée avant implémentation.

Le contrat graphique à préserver : scalaires admis, tableaux1/2 refusés avant mutation nativeGL, noms natifs observables conservés, binding et bufferleurre intacts après rejet. Une éventuelle tranche auteur devra garder RED causal, GREEN minimal et non-régression sur les deux environnements effectivement disponibles, sans prétendre qualifier tous3.3.

**Précision de frontière transmise à l'architecte :** le wrapper `IAttribute::set` appelle déjà `context.setValue<T>` avant le Setter ; le test graphique actuel ne vérifie pas de rollback du miroirCPU. La première formulation du brief mentionnant un miroir intact était donc trop générale. Ne pas promettre cette propriété depuis les preuves existantes ni élargir le correctif à une refonte du wrapper : ce constat préexistant doit être distingué de la classification et d'une éventuelle garantie nouvelle à contractualiser séparément.

## Parallélisme et état CI

L'architecte n'utilise ni build ni GPU. Le producteur conserve ces ressources et la CI distante peut continuer en parallèle. La correction du helper a été annoncée exécutée en normal avec bilanFAIL,75cas graphiques/3échecs et propagation du code8 ; cette nouvelle sortie reste à recouper à la remise finale. Les originauxINCONCLUSIVE sont conservés.

Au démarrage de l'avis, l'architecte a signalé QMD absent de son catalogue et un refus de lecture de la politique Context Mode hors projet. Le refus n'a pas été répété : le lead a fourni une copie locale de cette politique et les extraits documentaires déjà récupérés via artist-wiki, avec attribution explicite. La recherche QMD du spécialiste resteNOT_RUN ; cela ne signifie pas que les notes n'existent pas. Aucune configuration MCP ou permission n'est modifiée et la copie ne vaut pas élargissement du mandat de lecture seule. L'architecte a confirmé séparément le constat sur l'ordre setValue/Setter et distingue ce gapCPU préexistant du défaut de classification.

Le dernier état CI rapporté est encore en cours ; aucun nouveau contrôle distant ni PASS distant n'est revendiqué dans ce rapport. Gaps, audit et clôture documentaire restent différés jusqu'aux livrables intégrés.

## Annexe technique

- Sonde auteur : `/home/artist/src/ARTIST-integration/out/linux-evidence/20261028-attribute-name-probe/`.
- Preuves copiées et hashes recoupés par le lead : `out/logs/mission-linux-qa/attribute-name-probe/{source.cpp,build.log,run.log,manifest.json}`. Aucun rerun par le lead.
- Source SHA256 `76adb5b30d315108449ecbafebfccc791b03a2aec8dc8e4d2424c77cd82b677c` ; log `df1435dc59239d7e0cb2b8a2136276ee68254ee0177aebb4cce84dbe548a6eaf` ; manifeste `b2adf54c72787b9f628899c9bf13479ccf9e0ac6082373a7b7f9e48fbbbf1dad`.
- Commande compilateur exacte dans le manifeste, gcc14, bibliothèques GLFW/GLEW déjà présentes ; build0/run0 rapportés, compilation/link et observations confirmés dans le log relu. Binaire identifié par le manifeste, pas réexécuté par le lead.
- Avis : `task_mvsfp3ei_001o_j80gqi`, engine-architect/pool-7 effectivement routé, fresh sans worktree, completionlead. PM/LLM non-développement complexité68/risque72, incertitudeportabilité3.3, aucun override de modèle.
- Diagnostic antérieur accepté/intégré : `task_mvseouf3_0012_cb89ee`, rapport14. Codeexécutable toujours celui de a4c0f82 ; commitslead depuis cette base documentaires seulement.
