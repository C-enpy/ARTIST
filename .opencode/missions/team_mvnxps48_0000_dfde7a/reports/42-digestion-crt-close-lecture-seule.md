# 42 — Digestion CRT installée : fermeture d'un flux d'entrée

2026-10-26. Demande C reçue : discriminer l'hypothèse unique « fclose réconcilie le buffer d'entrée par seek/flush et peut échouer après lecture ». Lecture locale lead uniquement, aucun code/build/test ou flag/handle manipulé.

## Sources et identité observée

Racine trouvée par glob ciblé : `C:/Program Files (x86)/Windows Kits/10/Source/10.0.26100.0/ucrt/`. Sources également présentes pour19041/22621, non analysées ici. Le cache existant `out/build/windows-msvc-debug/CMakeCache.txt:189` sélectionne rc.exe du SDK10.0.26100.0. **Cela identifie le SDK disponible/sélectionné pour rc, pas l'identité octet-à-octet de la DLL CRT Debug effectivement chargée à l'exécution**. Aucune correspondance source/DLL ni exécution réelle de close nouvellement démontrée.

Empreintes SHA256 calculées par le lead sur les fichiers installés :
- stdio/fclose.cpp : 229a6c1780f188d784f0d42e5d7d57cfb5b1cee77bb399d0bdf7c0b34402a944.
- lowio/close.cpp : 66ef4ca9584690771768589c039ba04e024ad6fa5758958473e6bce4209603f3.
- stdio/fflush.cpp : 8ea13e8a7529503a561e32154d27c7e7864018df6b12133216a51419961ea8b4.

## Extraits ciblés et chaîne observée

fclose.cpp17–43 : `_fclose_nolock_internal` valide le stream ; si en usage, appelle directement `__acrt_stdio_flush_nolock`, libère son buffer, appelle `_close_internal`, fixe EOF en cas d'échec ; libère le stream même sur cette issue. **Pas appel de `_fflush_nolock_internal` dans ce chemin.** Extrait27–30 :

```cpp
result = __acrt_stdio_flush_nolock(stream.public_stream(), ptd);
__acrt_stdio_free_buffer_nolock(stream.public_stream());
if (_close_internal(_fileno(stream.public_stream()), ptd) < 0)
```

fflush.cpp18–30 définit is_stream_flushable ;226–229 retourne succès immédiatement si non flushable. La condition rejette les flux d'entrée :

```cpp
if ((stream_flags & (_IOREAD | _IOWRITE)) != _IOWRITE)
{
    return false;
}
```

La branche231–254 écrit un buffer de sortie ; aucun seek de réconciliation du buffer d'entrée dans le chemin lu. `_fflush_nolock_internal`185–211 peut, séparément, demander `_commit` sous `_IOCOMMIT`, mais ce n'est pas l'appel direct de fclose27. Ne pas fusionner ces deux chemins.

close.cpp15–45 : pour un descripteur fichier ordinaire valide, le helper appelle CloseHandle puis retourne GetLastError en cas de refus. Les exceptions de gestion stdout/stderr ne concernent pas ce fichier de shader. Extrait40–45 :

```cpp
if (CloseHandle(reinterpret_cast<HANDLE>(_get_osfhandle(fh))))
{
    return 0;
}
return GetLastError();
```

`_close_nolock_internal`81–94 appelle ce helper, libère l'association fd/handle et marque le fd fermé **avant** de mapper l'éventuelle erreur OS vers errno/retour -1. Couplé à fclose41, un échec ne permet donc pas de réessayer fclose sur le même FILE*. Aucun handle invalide/double-close créé pour confirmer ce constat statique.

## Disposition pour C et limites

La source installée lue ne soutient pas un échec fclose provenant d'un seek/flush de buffer d'entrée pour un flux strictement lecture seule. C doit qualifier cette conclusion pour cette source, pas impossibilité universelle ni preuve d'exécution de la DLL actuelle. Reste à identifier un refus authentique de CloseHandle sur owner/handle valides admissible au contrat, ou à nommer le manque exact.

La piste distincte HANDLE_FLAG_PROTECT_FROM_CLOSE a été signalée par C mais **n'est pas autorisée ni examinée dans cette hypothèse** : elle nécessite arbitrage sur l'interdiction des flags fabriquant une preuve et sur la récupération du handle après consommation CRT. Aucun flag appliqué, handle extrait, essai ou adoption implicite. Ne pas substituer cette piste sans décision explicite ; C termine d'abord la première hypothèse avec ces extraits et restitue ses limites.
