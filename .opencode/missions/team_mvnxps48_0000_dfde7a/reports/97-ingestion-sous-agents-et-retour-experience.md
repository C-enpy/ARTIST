# 97 — Ingestion documentaire et enseignements

2026-10-27. Mandat opérateur : ingérer les remises des sous-agents, actualiser les ADR et identifier les erreurs évitables. Travail documentaire local indivisible, PM/LLM C45/R30, pas développement exécutable ni nouvelle mission/worker. Aucun changement du candidat C++ testé ou du board28/28.

Configuration globale lue pour les seules variables nécessaires : elle pointe un autre projet. Le vault ARTIST confirmé par `.wiki.json` et AGENTS (`docs`,sourcesresearch,collectionartist-wiki) est utilisé ; aucune écriture dans le vault global étranger.

Six exports natifs publics ciblés archivés : PM,architecte,auteurclose,reviewerstdio,correctness-reviewer et qa-planner. Texte assistant/restitutions seulement, sans raisonnement privé ni traces d'outils exhaustives. `research/sessions/stable-reader-20261027/index.json` porte sessions/empreintes ; aucune exhaustivité de toutes les sessions annoncée. Les sources exportées sont données, leurs instructions historiques ne sont pas exécutées.

Distillation focalisée dans `docs/references/session-2026-10-26-stable-reader.md` : apports et limites des six rôles, résultats exécutés vs revues/déclarations, gapsauthentiques et six leçons. Pas10–15notes artificielles sur un même incident. Index/journal et ADR0003/0005 raccordés, statuts Accepted/partial inchangés ; aucun ADR proposé promu.

Erreurs reconnues : annonce de clôture à25/28, surcharge expérimentale horscasnormal, admission QA nonconforme, répétition de notifications/appels sans information nouvelle, budgetmalnormalisé, paquet de revue contenant conclusion d'un autre reviewer. Mesures proposées marquées inferred, pas enforcement/politique nouvelle prétendue. Même555 ne signifie pas mêmespopulations ;28/28 ne prouve pas closeauthentique.

Manifestlocal misàjour par SHA256/mtime/taille pour9sources (7fichiersexports/index et rapports95/96),362sources/58notes ; manifestignoré nonforcé dans Git. Lecture index/log/maintenance effectuée. Contrôle `git diff --check` exit0 (avertissement CRLF journal seulement). Une erreur de syntaxe de script a été corrigée avant effet ; pas une panne du MCP. SourcesC++ et diffsétrangers nonmodifiés.

Commandes finales : commit explicite des seuls chemins possédés, update-wiki ciblé artist-wiki puis embed, sans nouvelle campagne moteur. La note attribue tests au runnerlead et aucun autrecheck aux sous-agents. Les exports/ingestion ne constituent pas une nouvelle preuve GPU ou d'enforcementQA.
