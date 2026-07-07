// VampireSurvivors.Runtime, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null
// VampireSurvivors.Objects.Projectiles.MagicMissileProjectile
using System;
using System.Runtime.CompilerServices;
using Unity.Profiling;
using Unity.Profiling.LowLevel;
using Unity.Profiling.LowLevel.Unsafe;
using UnityEngine;
using UnityEngine.Bindings;
using VampireSurvivors.Framework.Particles;
using VampireSurvivors.Interfaces;
using VampireSurvivors.Objects.Pools;
using VampireSurvivors.Objects.Projectiles;
using VampireSurvivors.Objects.Weapons;

public class MagicMissileProjectile : Projectile
{
	private float _IndexOffsetScaleFactor;

	private ParticleEmitterManager _pfxManager;

	private ParticleSystem _pfx;

	private MagicMissileWeapon _trueWeapon;

	private static readonly ProfilerMarker _markerInitProjectile;

	public override void InitProjectile(BulletPool pool, Weapon weapon, int index)
	{
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Expected O, but got Unknown
		//IL_018c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0191: Expected O, but got Unknown
		//IL_01a8: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ad: Expected O, but got Unknown
		//IL_01df: Expected O, but got I4
		//IL_00dd: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e2: Expected O, but got Unknown
		//IL_00f9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00fe: Expected O, but got Unknown
		//IL_0254: Unknown result type (might be due to invalid IL or missing references)
		//IL_0259: Expected O, but got Unknown
		//IL_0270: Unknown result type (might be due to invalid IL or missing references)
		//IL_0275: Expected F4, but got Unknown
		//IL_02c8: Expected O, but got I4
		//IL_02c8: Expected F4, but got O
		//IL_02e8: Expected O, but got I4
		//IL_03a0: Expected I, but got O
		//IL_0378: Expected O, but got I4
		//IL_038a: Expected O, but got I4
		//IL_0490: Expected O, but got I4
		//IL_0447: Expected O, but got I
		//IL_049e: Expected O, but got I4
		//IL_04ea: Expected I4, but got O
		//IL_04f7: Unknown result type (might be due to invalid IL or missing references)
		//IL_04fc: Expected O, but got Unknown
		//IL_051b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0520: Expected O, but got Unknown
		//IL_0587: Expected O, but got I8
		//IL_05ac: Expected I, but got O
		//IL_05b5: Expected O, but got I4
		//IL_063f: Expected I, but got O
		//IL_065d: Expected O, but got I4
		//IL_0993: Unknown result type (might be due to invalid IL or missing references)
		//IL_0998: Expected F4, but got Unknown
		//IL_0ac6: Expected O, but got I8
		//IL_0520->IL0520: Incompatible stack heights: 7 vs 6
		//IL_057a->IL0154: Incompatible stack heights: 6 vs 1
		//IL_077b->IL0211: Incompatible stack heights: 8 vs 1
		//IL_0801->IL02e8: Incompatible stack heights: 9 vs 6
		//IL_0801->IL0801: Incompatible stack heights: 9 vs 8
		//IL_08e2->IL05c2: Incompatible stack heights: 8 vs 7
		//IL_088f->IL0339: Incompatible stack heights: 9 vs 6
		//IL_088f->IL088f: Incompatible stack heights: 9 vs 8
		//IL_094f->IL05c2: Incompatible stack heights: 9 vs 7
		//IL_09e5->IL0490: Incompatible stack heights: 9 vs 6
		//IL_09a9->IL09a9: Incompatible stack heights: 10 vs 9
		//IL_0a4f->IL03db: Incompatible stack heights: 9 vs 6
		//IL_0afc->IL0154: Incompatible stack heights: 10 vs 1
		//IL_0afc->IL0afc: Incompatible stack heights: 10 vs 9
		bool flag = (long)new string("洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀\u3100㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀") != 0L;
		float num2 = default(float);
		float num = num2;
		object obj = default(object);
		if (!flag)
		{
			Console.WriteLine("匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀");
			obj = num2 | 0L;
			Console.WriteLine("欀倀爀漀瘀椀搀攀爀㨀㨀伀渀倀㈀倀匀攀猀猀椀漀渀刀攀愀搀礀䤀渀瘀漀欀攀\0啅渀洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀");
		}
		Console.WriteLine("Unknown operand: Method: VampireSurvivors.Objects.Projectiles.Projectile::InitProjectile");
		MagicMissileProjectile magicMissileProjectile = default(MagicMissileProjectile);
		((Projectile)magicMissileProjectile).InitProjectile(pool, weapon, index);
		MagicMissileProjectile magicMissileProjectile2 = (MagicMissileProjectile)(object)new string("愀砀开瘀㐀\u2000⠀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䤀䰀椀猀琀怀\u3100㰀倀氀愀礀䘀愀戀⸀倀愀爀琀礀⸀倀");
		bool flag2 = new string("愀砀开瘀㐀\u2000⠀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䤀䰀椀猀琀怀\u3100㰀倀氀愀礀䘀愀戀⸀倀愀爀琀礀⸀倀") == null;
		Weapon weapon2 = weapon;
		int num3 = index;
		BulletPool bulletPool = pool;
		MagicMissileProjectile magicMissileProjectile3 = (MagicMissileProjectile)(object)new string("愀砀开瘀㐀\u2000⠀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䤀䰀椀猀琀怀\u3100㰀倀氀愀礀䘀愀戀⸀倀愀爀琀礀⸀倀");
		Weapon weapon3 = weapon;
		object obj3 = default(object);
		if (!flag2)
		{
			object obj2 = obj | 0L;
			Console.WriteLine("渀搀\u2000昀漀爀㨀\u2000吀礀瀀攀㨀\u2000匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䤀䌀漀氀氀攀挀琀椀漀渀怀\u3100㰀倀氀愀");
			obj3 = obj2 | 0L;
			Console.WriteLine("渀搀㨀\u2000䴀攀琀栀漀搀㨀\u2000倀氀愀礀䘀愀戀⸀倀愀爀琀礀⸀倀氀愀礀䘀愀戀䴀甀氀琀椀瀀氀愀礀攀爀䴀愀渀愀");
			bool flag3 = new string("") == null;
			weapon2 = weapon;
			num3 = index;
			bulletPool = pool;
			magicMissileProjectile3 = (MagicMissileProjectile)(object)new string("");
			weapon3 = weapon;
			if (flag3)
			{
			}
		}
		float? markerCleanup = default(float?);
		Weapon weapon10 = default(Weapon);
		Weapon weapon11 = default(Weapon);
		int num14 = default(int);
		int num15 = default(int);
		Weapon weapon12 = default(Weapon);
		object obj9 = default(object);
		Weapon weapon13 = default(Weapon);
		int num16 = default(int);
		Weapon weapon14 = default(Weapon);
		Weapon weapon16 = default(Weapon);
		BulletPool bulletPool4 = default(BulletPool);
		MagicMissileProjectile magicMissileProjectile5 = default(MagicMissileProjectile);
		float? num21 = default(float?);
		nint num22 = default(nint);
		Weapon trueWeapon = default(Weapon);
		MagicMissileProjectile magicMissileProjectile6 = default(MagicMissileProjectile);
		object obj14 = default(object);
		object obj16 = default(object);
		object obj18 = default(object);
		MagicMissileProjectile magicMissileProjectile7 = default(MagicMissileProjectile);
		while (true)
		{
			Console.WriteLine("Method not found @180AB7550");
			Weapon weapon4 = weapon2;
			int num4 = num3;
			BulletPool bulletPool2 = bulletPool;
			Weapon weapon5 = weapon3;
			object obj4 = obj3 | 0L;
			Console.WriteLine("氀愀礀攀爀䴀愀渀愀最攀爀⬀伀渀一攀琀眀漀爀欀䨀漀椀渀攀搀䠀愀渀搀氀攀爀⤀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀");
			object obj5 = obj4 | 0L;
			Console.WriteLine("漀眀渀\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000\u3100㠀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀");
			Console.WriteLine("洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㔀\u3100\u2000䀀\u2000爀㤀开瘀\u3100⬀㠀⬀瘀㜀\u3000\u2000䀀\u2000爀愀砀开瘀㘀崀\0啗渀洀愀渀");
			bool flag4 = new string("") == null;
			weapon2 = (Weapon)0;
			num3 = 0;
			bulletPool = (BulletPool)_markerInitProjectile;
			magicMissileProjectile3 = (MagicMissileProjectile)(object)new string("");
			weapon3 = weapon;
			if (flag4)
			{
				continue;
			}
			while (true)
			{
				IL_0211:
				Console.WriteLine("Method not found @180AB7B30");
				Console.WriteLine("愀砀开瘀㈀㈀崀\0啙渀洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㈀\u3000\u3100\u2000䀀\u2000爀搀砀开瘀㜀⬀㐀㘀㈀䔀\u3000崀\0\udb80War");
				Weapon weapon6 = weapon4;
				int num5 = num4;
				int num6 = 0;
				Weapon weapon7 = weapon5;
				object obj6 = obj5 | 0L;
				Console.WriteLine("");
				float num7 = obj6 | 0L;
				_ = 1L;
				num = num7;
				ProfilerMarker markerInitProjectile = _markerInitProjectile;
				object obj7 = new string("挀漀搀攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀\u3100㘀\u2000䀀\u2000爀愀砀");
				Console.WriteLine("");
				ArcadeSprite arcadeSprite = magicMissileProjectile.setScale((float)new string("搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀\u3100㠀㤀㤀䄀㐀㈀㈀㈀崀\0ւUnknown call target op"), (float?)(object)0);
				Console.WriteLine("最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀\u3000\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000洀攀");
				((MagicMissileProjectile)0).GenerateParticleSystem();
				while (true)
				{
					IL_02e8:
					Console.WriteLine("Method not found @180AB7B30");
					Console.WriteLine("");
					Weapon weapon8 = weapon6;
					int num8 = num5;
					int num9 = 0;
					Weapon weapon9 = weapon7;
					if ((object)weapon == null)
					{
						goto IL_0339;
					}
					nint num10 = (nint)new MagicMissileWeapon();
					markerCleanup = (float?)Weapon._markerCleanup;
					object obj8 = new string("");
					string text = new string("");
					string text2 = new string("");
					if (System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text) >= System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text2))
					{
						goto IL_03db;
					}
					goto IL_0487;
					IL_094f:
					bool flag5 = (long)new string("Unmanaged memory load: [189999AE1]") != 0L;
					float num12;
					float num11 = num12;
					if (!flag5)
					{
						Console.WriteLine("\u2000爀搀砀\u2000⠀匀礀猀琀攀洀⸀匀琀爀椀渀最⤀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000");
						float num13 = num12 | 0L;
						_ = 1L;
						num11 = num13;
					}
					bool flag6 = new string("") == null;
					weapon10 = weapon11;
					num14 = num15;
					weapon12 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
					int num17;
					if (!flag6)
					{
						if (new string("Unmanaged memory load: [189999B38]") == null)
						{
							Console.WriteLine("䴀愀渀愀最攀爀㸀⤀⬀䔀㐀崀\0辁Unknown call target operand: 66 Call \"il2");
							bool flag7 = obj9 == null;
							weapon13 = weapon11;
							num16 = num15;
							num17 = 0;
							weapon14 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
							if (flag7)
							{
								goto IL_03db;
							}
						}
						Console.WriteLine("礀猀琀攀洀⸀匀琀爀椀渀最⤀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\0쾀Unmana");
						ProfilerMarker markerInitProjectile2 = _markerInitProjectile;
						Console.WriteLine("椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀椀挀欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀㸀⤀⬀䈀㠀崀\0啿渀洀愀渀愀最");
						if ((long)new string("琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000") < 7L)
						{
							object obj10 = new string("");
							bool flag8 = new string("") == null;
							weapon2 = (Weapon)1L;
							num3 = 0;
							bulletPool = (BulletPool)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
							magicMissileProjectile3 = magicMissileProjectile;
							weapon3 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
							if (flag8)
							{
								break;
							}
						}
						return;
					}
					goto IL_0490;
					IL_0487:
					MagicMissileProjectile magicMissileProjectile4 = (MagicMissileProjectile)0;
					goto IL_0490;
					IL_0490:
					UnityEngine.Bindings.ThrowHelper.ThrowNullReferenceException(0);
					Weapon weapon15 = weapon10;
					int num18 = num14;
					int num19 = 0;
					object obj11 = weapon12;
					weapon16 = weapon12;
					object obj12 = (object)_trueWeapon >> 12;
					int num20 = (int)new string("Unmanaged memory load: [1899EFB60]");
					object obj13 = obj12 & 0x1FFFFFL;
					BulletPool bulletPool3 = (BulletPool)(obj13 >> 6);
					Weapon weapon17 = (Weapon)(obj13 & 0x3FL);
					goto IL_0520;
					IL_0520:
					Weapon weapon18 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
					bool flag9 = new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀") == null;
					weapon2 = weapon11;
					num3 = num15;
					bulletPool = bulletPool4;
					magicMissileProjectile3 = magicMissileProjectile5;
					weapon3 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
					if (flag9)
					{
						break;
					}
					magicMissileProjectile4 = (MagicMissileProjectile)1L;
					bool flag10 = (object)magicMissileProjectile4 == null;
					num21 = markerCleanup;
					num22 = (nint)new MagicMissileWeapon();
					trueWeapon = (Weapon)0;
					magicMissileProjectile6 = magicMissileProjectile4;
					while (true)
					{
						Console.WriteLine("Method not found @180AB7550");
						Weapon weapon19 = weapon16;
						Console.WriteLine("Not implemented instruction: bts rcx,r8");
						bool flag11 = (object)new string("") != new string("");
						bool flag12 = (long)new string("Unmanaged memory load: [189999AE0]") != 0L;
						num12 = num;
						if (!flag12)
						{
							num21 = markerCleanup;
							num22 = (nint)new MagicMissileWeapon();
							trueWeapon = weapon;
							magicMissileProjectile6 = magicMissileProjectile4;
						}
						else
						{
							bool flag13 = new string("开瘀\u3100\u3000\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀椀挀欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀") == null;
							weapon19 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
							if (!flag13)
							{
								if (new string("Unmanaged memory load: [189999B30]") == null)
								{
									Console.WriteLine("漀爀礀\u2000氀漀愀搀㨀\u2000嬀漀渀䰀漀愀搀攀搀\u2000䀀\u2000爀㠀\u2000⠀匀礀猀琀攀洀⸀䄀挀琀椀漀渀怀\u3100㰀䌀漀栀攀爀攀渀挀攀⸀吀漀");
									bool flag14 = obj14 == null;
									weapon4 = weapon11;
									num4 = num15;
									bulletPool2 = bulletPool4;
									weapon5 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
									if (flag14)
									{
										break;
									}
								}
								Console.WriteLine("");
								bool flag15 = new string("Unmanaged memory load: [1899990A0]") != null;
								object obj15 = new string("开瘀\u3100\u3000\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀椀挀欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀");
								if (!flag15)
								{
									Console.WriteLine("");
									bool flag16 = obj16 == null;
									weapon6 = weapon11;
									num5 = num15;
									num6 = 0;
									weapon7 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
									if (flag16)
									{
										goto IL_02e8;
									}
								}
								Console.WriteLine("");
								bool flag17 = new string("Unmanaged memory load: [1899990A0]") != null;
								object obj17 = obj15;
								if (!flag17)
								{
									obj15 = new string("Unmanaged memory load: [187C9B638]");
									Console.WriteLine("㤀\u2000䀀\u2000爀㤀\0춀Unmanaged memory");
									bool flag18 = obj18 == null;
									weapon8 = weapon11;
									num8 = num15;
									num9 = 0;
									weapon9 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
									if (flag18)
									{
										goto IL_0339;
									}
								}
								Console.WriteLine("");
								bool flag19 = (object)_trueWeapon == null;
								weapon15 = weapon11;
								num18 = num15;
								num19 = 0;
								obj11 = obj17;
								weapon16 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
								if (flag19)
								{
									continue;
								}
								obj17 = new string("Unmanaged memory load: [187C9B638]");
								Weapon weapon20 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
								bool flag20 = new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀") == null;
								weapon15 = weapon11;
								num18 = num15;
								num19 = 0;
								obj11 = obj17;
								weapon16 = (Weapon)(object)new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
								if (flag20)
								{
									continue;
								}
								goto IL_094f;
							}
						}
						UnityEngine.Bindings.ThrowHelper.ThrowNullReferenceException(0);
						return;
					}
					goto IL_0211;
					IL_0408:
					bool flag21 = (long)new string("Unmanaged memory load: [18996C9E0]") == 0L;
					magicMissileProjectile7._trueWeapon = (MagicMissileWeapon)trueWeapon;
					weapon11 = (Weapon)num21;
					num15 = 0;
					bulletPool4 = (BulletPool)num22;
					magicMissileProjectile5 = magicMissileProjectile6;
					if (!flag21)
					{
						object obj19 = new string("");
						if ((object)new string("") != new MagicMissileWeapon())
						{
						}
						goto IL_0487;
					}
					goto IL_0520;
					IL_03db:
					Console.WriteLine("Method not found @180AB7B30");
					Console.WriteLine("⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀椀挀欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀");
					weapon10 = weapon13;
					num14 = num16;
					weapon12 = weapon14;
					goto IL_0408;
					IL_0339:
					Console.WriteLine("Method not found @180AB7B30");
					Console.WriteLine("");
					weapon13 = weapon8;
					num16 = num8;
					num17 = 0;
					weapon14 = weapon9;
					num21 = (float?)(object)0;
					num22 = 0;
					trueWeapon = (Weapon)0;
					magicMissileProjectile6 = magicMissileProjectile;
					goto IL_0408;
				}
				break;
			}
		}
	}

	private void GenerateParticleSystem()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected override void OnHasHitAnObject(IDamageable other)
	{
		//IL_0024: Expected I4, but got I8
		Console.WriteLine("搀㨀\u2000㤀㌀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开爀甀渀琀椀洀攀开挀氀愀猀猀开椀渀椀琀开愀挀琀甀愀氀∀Ⰰ\u2000瘀㈀\u3100㠀\u2000䀀\u2000爀");
		MagicMissileProjectile magicMissileProjectile = default(MagicMissileProjectile);
		magicMissileProjectile.OnHasHitAnObjectLogic(other, triggerHit: true);
	}

	protected override void OnHasHitAnotherPlayerObject(IDamageable other)
	{
		//IL_0024: Expected I, but got O
		//IL_0088: Expected I, but got I8
		bool flag = (long)new string("") != 0L;
		MagicMissileProjectile magicMissileProjectile = default(MagicMissileProjectile);
		nint num = (nint)magicMissileProjectile;
		if (!flag)
		{
			Console.WriteLine("Unknown call target operand: 19 Call \"il2cpp_codegen_initialize_runtime_metadata\", v20 @ rax_v1, methodof(System.Collections.Generic.HashSet`1<VampireSurvivors.Interfaces.IDamageable>::Clear), other @ rdx (VampireSurvivors.Interfaces.IDamageable), v21 @ r8, v22 @ r9");
			Console.WriteLine("Unknown call target operand: 22 Call \"il2cpp_codegen_initialize_runtime_metadata\", v27 @ rax_v2, typeof(VampireSurvivors.Interfaces.IDamageable), other @ rdx (VampireSurvivors.Interfaces.IDamageable), v21 @ r8, v22 @ r9");
		}
		bool flag2 = other == null;
		object obj2 = default(object);
		object obj = obj2;
		IntPtr intPtr = num;
		if (!flag2)
		{
			Console.WriteLine("Constructor not found for: Type: VampireSurvivors.Interfaces.IDamageable (probably static type)");
			num = 1;
			Console.WriteLine("Method not found @180002A60");
			object obj3 = default(object);
			if (obj3 != null)
			{
				return;
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	private void OnHasHitAnObjectLogic(IDamageable other, bool triggerHit)
	{
		//IL_0024: Expected I, but got O
		//IL_0088: Expected I, but got I8
		bool flag = (long)new string("") != 0L;
		MagicMissileProjectile magicMissileProjectile = default(MagicMissileProjectile);
		nint num = (nint)magicMissileProjectile;
		if (!flag)
		{
			Console.WriteLine("Unknown call target operand: 21 Call \"il2cpp_codegen_initialize_runtime_metadata\", v24 @ rax_v1, methodof(System.Collections.Generic.HashSet`1<VampireSurvivors.Interfaces.IDamageable>::Clear), other @ rdx (VampireSurvivors.Interfaces.IDamageable), triggerHit @ r8 (System.Boolean), v25 @ r9");
			Console.WriteLine("Unknown call target operand: 24 Call \"il2cpp_codegen_initialize_runtime_metadata\", v30 @ rax_v2, typeof(VampireSurvivors.Interfaces.IDamageable), other @ rdx (VampireSurvivors.Interfaces.IDamageable), triggerHit @ r8 (System.Boolean), v25 @ r9");
		}
		bool flag2 = other == null;
		bool flag3 = triggerHit;
		IntPtr intPtr = num;
		if (!flag2)
		{
			Console.WriteLine("Constructor not found for: Type: VampireSurvivors.Interfaces.IDamageable (probably static type)");
			num = 1;
			Console.WriteLine("Method not found @180002A60");
			object obj = default(object);
			if (obj != null)
			{
				return;
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public override void InternalUpdate()
	{
		//IL_0041: Expected I, but got O
		//IL_00bb: Expected I, but got O
		//IL_0145: Expected I, but got O
		//IL_01cb: Expected I, but got O
		//IL_0287: Expected O, but got I4
		//IL_02d6: Expected O, but got I4
		//IL_02e4: Expected I, but got O
		//IL_0319: Expected I4, but got I8
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		//IL_00b2->IL00b2: Incompatible stack heights: 1 vs 0
		//IL_0122->IL0122: Incompatible stack heights: 1 vs 0
		//IL_01f9->IL021f: Incompatible stack heights: 0 vs 1
		//IL_02ed->IL01f9: Incompatible stack heights: 1 vs 0
		if ((long)new string("开渀攀眀开猀瀀攀挀椀昀椀挀∀Ⰰ\u2000瘀㐀㈀\u3100\u2000䀀\u2000爀愀砀开瘀㌀㈀\u2000⠀匀礀猀琀攀洀⸀䈀礀琀攀嬀崀⤀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀攀") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 18 Call \"il2cpp_codegen_initialize_runtime_metadata\", v18 @ rax_v1, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num = (nint)new UnityEngine.Object();
		ParticleSystem pfx = _pfx;
		if ((long)new string("Unmanaged memory load: [v28 @ rcx_v3 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 35 Call \"il2cpp_runtime_class_init_actual\", v41 @ rax_v3, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [18999958F]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 49 Call \"il2cpp_codegen_initialize_runtime_metadata\", v55 @ rax_v5, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num2 = (nint)new UnityEngine.Object();
		if ((long)new string("Unmanaged memory load: [v60 @ rcx_v6 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 65 Call \"il2cpp_runtime_class_init_actual\", v72 @ rax_v7, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [189999551]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 79 Call \"il2cpp_codegen_initialize_runtime_metadata\", v86 @ rax_v9, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		object obj2;
		if ((object)_pfx != null)
		{
			nint num3 = (nint)new UnityEngine.Object();
			if ((long)new string("Unmanaged memory load: [v101 @ rcx_v10 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L || (long)new string("Unmanaged memory load: [v29 @ rdi_v1 (UnityEngine.ParticleSystem)+10]") == 0L)
			{
				return;
			}
			object obj = new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
			bool flag = new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀") == null;
			object obj3 = default(object);
			obj2 = obj3;
			nint num4 = (nint)new UnityEngine.Object();
			if (flag || (long)new string("Unmanaged memory load: [189999AE0]") == 0L)
			{
				goto IL_01f9;
			}
			goto IL_021f;
		}
		return;
		IL_021f:
		if (new string("Unmanaged memory load: [v161 @ rbx_v3 (System.Object)+10]") != null)
		{
			Console.WriteLine("Method not found @180AB7B30");
			Console.WriteLine("Unknown call target operand: 228 Call \"il2cpp_vm_exception_raise\", v321 @ rax_v24, v351 @ rax_v23, 0, v20 @ r8, v21 @ r9");
			if (new string("Unmanaged memory load: [189999B30]") != null)
			{
				Console.WriteLine("Indirect call: 192 IndirectCall v154 @ rax_v19 (should have been resolved before IL gen)");
				bool flag2 = (object)_pfxManager == null;
				obj2 = 0;
				nint num4 = (nint)new string("Unmanaged memory load: [v161 @ rbx_v3 (System.Object)+10]");
				if (!flag2)
				{
					_ = _pfxManager;
					Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.Particles.ParticleEmitterManager::EmitParticleAt");
					Vector2 pos = default(Vector2);
					_pfxManager.EmitParticleAt(pos, 1);
					Console.WriteLine("Warning: Method ends with non empty stack (-38), the output could be wrong!");
					/*Error: End of method reached without returning.*/;
				}
				goto IL_01f9;
			}
		}
		new string("欀甀瀀猀⸀倀椀挀欀甀瀀䴀愀渀愀最攀爀⤀Ⰰ\u2000渀攀琀眀漀爀欀䄀猀猀攀琀䤀搀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀");
		UnityEngine.Bindings.ThrowHelper.ThrowNullReferenceException(0);
		return;
		IL_01f9:
		Console.WriteLine("Method not found @180AB7550");
		object obj4 = obj2;
		Console.WriteLine("Unknown call target operand: 145 Call \"il2cpp_codegen_initialize_runtime_metadata\", v288 @ rax_v14, methodof(UnityEngine.Object+MarshalledUnityObject::MarshalNotNull), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		_ = 1L;
		goto IL_021f;
	}

	public MagicMissileProjectile()
	{
		//IL_004a: Expected F4, but got I8
		//IL_0053: Expected I, but got O
		//IL_009e: Expected O, but got I4
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 17 Call \"il2cpp_codegen_initialize_runtime_metadata\", v16 @ rax_v1, typeof(VampireSurvivors.Objects.Projectiles.Projectile), methodInfo @ rdx, v18 @ r8, v19 @ r9");
			_ = 1L;
		}
		MagicMissileProjectile magicMissileProjectile = default(MagicMissileProjectile);
		magicMissileProjectile._IndexOffsetScaleFactor = 1.03683194E+09f;
		nint num = (nint)new Projectile();
		if ((long)new string("Unmanaged memory load: [v26 @ rcx_v3 (Il2CppClass<VampireSurvivors.Objects.Projectiles.Projectile>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 34 Call \"il2cpp_runtime_class_init_actual\", v38 @ rax_v3, typeof(VampireSurvivors.Objects.Projectiles.Projectile), methodInfo @ rdx, v18 @ r8, v19 @ r9");
		}
		Console.WriteLine("Unknown operand: Method: VampireSurvivors.Objects.Projectiles.Projectile::.ctor");
		((Projectile)0)..ctor();
	}

	static MagicMissileProjectile()
	{
		//IL_0074: Expected I4, but got O
		//IL_0074: Expected O, but got I8
		//IL_0081: Expected I, but got O
		//IL_0097: Expected O, but got I
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Expected O, but got Unknown
		bool flag = (long)new string("琀攀洀⸀䈀礀琀攀嬀崀⤀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀攀洀⸀䈀礀琀攀嬀\u3100崀⤀Ⰰ\u2000瘀㌀㤀㐀\u2000䀀\u2000爀愀砀开瘀㌀\u3100Ⰰ\u2000瘀㌀\u3100\u2000") != 0L;
		object obj2 = default(object);
		object obj = obj2;
		if (!flag)
		{
			Console.WriteLine("Ⰰ\u2000瘀㌀㤀㐀\u2000䀀\u2000爀愀砀开瘀㌀\u3100Ⰰ\u2000瘀㌀");
			object obj3 = obj2 | 0L;
			Console.WriteLine("䀀\u2000爀愀砀开瘀㌀\u3100Ⰰ\u2000瘀㌀\u3100\u2000䀀");
		}
		IntPtr intPtr = ProfilerUnsafeUtility.CreateMarker((string)1L, 0, MarkerFlags.Default, (int)obj);
		nint num = (nint)new MagicMissileProjectile();
		object obj4 = new string("瀀瀀开挀漀搀攀最攀渀开漀戀樀攀挀琀开渀攀眀∀Ⰰ\u2000瘀㐀㔀㜀\u2000䀀\u2000爀愀砀开瘀㈀㤀\u2000⠀匀礀猀琀攀洀⸀䄀爀");
		obj4 = (nint)intPtr;
	}
}
